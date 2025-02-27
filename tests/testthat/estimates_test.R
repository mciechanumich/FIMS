# Load necessary data for the integration test
load(test_path("fixtures", "integration_test_data.RData"))

# Set the iteration ID to 1 for accessing specific input/output list
iter_id <- 1

# Extract model input and output data for the specified iteration
om_input <- om_input_list[[iter_id]]
om_output <- om_output_list[[iter_id]]
em_input <- em_input_list[[iter_id]]

# Define modified parameters for different modules
modified_parameters <- vector(mode = "list", length = length(iter_id))
modified_parameters[[iter_id]] <- list(
  fleet1 = list(
    Fleet.log_Fmort.value = log(om_output_list[[iter_id]][["f"]])
  ),
  survey1 = list(
    LogisticSelectivity.inflection_point.value = 1.5,
    LogisticSelectivity.slope.value = 2,
    Fleet.log_q.value = log(om_output_list[[iter_id]][["survey_q"]][["survey1"]])
  ),
  recruitment = list(
    BevertonHoltRecruitment.log_rzero.value = log(om_input_list[[iter_id]][["R0"]]),
    BevertonHoltRecruitment.log_devs.value = om_input_list[[iter_id]][["logR.resid"]][-1],
    # TODO: integration tests fail after setting BevertonHoltRecruitment.log_devs.estimated
    # to TRUE. We need to debug the issue, then update the line below accordingly.
    BevertonHoltRecruitment.log_devs.estimated = FALSE,
    DnormDistribution.log_sd.value = om_input_list[[iter_id]][["logR_sd"]]
  ),
  maturity = list(
    LogisticMaturity.inflection_point.value = om_input_list[[iter_id]][["A50.mat"]],
    LogisticMaturity.inflection_point.estimated = FALSE,
    LogisticMaturity.slope.value = om_input_list[[iter_id]][["slope.mat"]],
    LogisticMaturity.slope.estimated = FALSE
  ),
  population = list(
    Population.log_init_naa.value = log(om_output_list[[iter_id]][["N.age"]][1, ])
  )
)

estimation_mode <- TRUE
map <- list()

# Load operating model data for the current iteration
om_input <- om_input_list[[iter_id]]
om_output <- om_output_list[[iter_id]]
em_input <- em_input_list[[iter_id]]

# Clear any previous FIMS settings
clear()

data <- FIMS::FIMSFrame(data1)

# Set up default parameters
fleets <- list(
  fleet1 = list(
    selectivity = list(form = "LogisticSelectivity"),
    data_distribution = c(
      Index = "DlnormDistribution",
      AgeComp = "DmultinomDistribution",
      LengthComp = "DmultinomDistribution"
    )
  ),
  survey1 = list(
    selectivity = list(form = "LogisticSelectivity"),
    data_distribution = c(
      Index = "DlnormDistribution",
      AgeComp = "DmultinomDistribution",
      LengthComp = "DmultinomDistribution"
    )
  )
)

default_parameters <- data |>
  create_default_parameters(
    fleets = fleets,
    recruitment = list(
      form = "BevertonHoltRecruitment",
      process_distribution = c(log_devs = "DnormDistribution")
    ),
    growth = list(form = "EWAAgrowth"),
    maturity = list(form = "LogisticMaturity")
  )

parameters <- default_parameters |>
  update_parameters(
    modified_parameters = modified_parameters[[iter_id]]
  )

parameter_list <- initialize_fims(
  parameters = parameters,
  data = data
)

input <- parameter_list
get_sd <- TRUE
save_sd <- TRUE
number_of_loops <- 3
optimize <- TRUE
number_of_newton_steps <- 0
control <- list(
  eval.max = 10000,
  iter.max = 10000,
  trace = 0
)
filename <- NULL

if (!is.null(input$random)) {
  cli::cli_abort("Random effects declared but are not implemented yet.")
}

if (number_of_newton_steps > 0) {
  cli::cli_abort("Newton steps not implemented yet.")
}
if (number_of_loops < 0) {
  cli::cli_abort("number_of_loops ({.par {number_of_loops}}) must be >= 0.")
}
obj <- TMB::MakeADFun(
  data = list(),
  parameters = input$parameters,
  map = input$map,
  random = input$random,
  DLL = "FIMS",
  silent = TRUE
)
if (!optimize) {
  initial_fit <- FIMSFit(
    input = input,
    obj = obj,
    timing = c("time_total" = as.difftime(0, units = "secs"))
  )
  return(initial_fit)
}
if (!is_fims_verbose()) {
  control$trace <- 0
}
## optimize and compare
cli::cli_inform(c("v" = "Starting optimization ..."))
t0 <- Sys.time()
opt <- with(
  obj,
  nlminb(
    start = par,
    objective = fn,
    gradient = gr,
    control = control
  )
)
maxgrad0 <- maxgrad <- max(abs(obj$gr(opt$par)))
if (number_of_loops > 0) {
  cli::cli_inform(c(
    "i" = "Restarting optimizer {number_of_loops} times to improve gradient."
  ))
  for (ii in 1:number_of_loops) {
    # control$trace is reset to zero regardless of verbosity because the
    # differences in values printed out using control$trace will be
    # negligible between these different runs and is not worth printing
    control$trace <- 0
    opt <- with(
      obj,
      nlminb(
        start = opt[["par"]],
        objective = fn,
        gradient = gr,
        control = control
      )
    )
    maxgrad <- max(abs(obj[["gr"]](opt[["par"]])))
  }
  div_digit <- cli::cli_div(theme = list(.val = list(digits = 5)))
  cli::cli_inform(c(
    "i" = "Maximum gradient went from {.val {maxgrad0}} to
            {.val {maxgrad}} after {number_of_loops} steps."
  ))
  cli::cli_end(div_digit)
}
time_optimization <- Sys.time() - t0
cli::cli_inform(c("v" = "Finished optimization"))

time_sdreport <- NA
if (get_sd) {
  t2 <- Sys.time()
  sdreport <- TMB::sdreport(obj)
  cli::cli_inform(c("v" = "Finished sdreport"))
  time_sdreport <- Sys.time() - t2
} else {
  sdreport <- list()
  time_sdreport <- as.difftime(0, units = "secs")
}

timing <- c(
  time_optimization = time_optimization,
  time_sdreport = time_sdreport,
  time_total = Sys.time() - t0
)

version <- utils::packageVersion("FIMS")

# What we aspire the estimate table to look like
estimates_outline <- dplyr::tibble(
  label = character(),
  fleet = character(),
  age = numeric(),
  length = numeric(),
  time = numeric(),
  initial = numeric(),
  estimate = numeric(),
  uncertainty = numeric(),
  likelihood = numeric(),
  gradient = numeric(),
  estimated = logical()
)
rm(estimates_outline)

# Determine the number of parameters
n_total <- length(obj[["env"]][["last.par.best"]])
n_fixed_effects <- length(obj[["par"]])
number_of_parameters <- c(
  total = n_total,
  fixed_effects = n_fixed_effects,
  random_effects = n_total - n_fixed_effects
)
rm(n_total, n_fixed_effects)

# Calculate the maximum gradient
max_gradient <- if (length(opt) > 0) {
  max(abs(obj[["gr"]](opt[["par"]])))
} else {
  NA_real_
}

# Rename parameters instead of "p"
parameter_names <- names(get_parameter_names(obj[["par"]]))
names(obj[["par"]]) <- parameter_names

# Get the report
report <- if (length(opt) == 0) {
  obj[["report"]](obj[["env"]][["last.par.best"]])
} else {
  obj[["report"]]()
}

###################################################
estimates <- estimates_outline |>
  add_row(
    label = dimnames(std)[[1]],
    estimate = std[, "Estimate"],
    uncertainty = std[, "Std. Error"]
  ) |>
  # initial: the initial value use to start the optimization procedure
  # Use obj[["env"]][["parameters"]][["p"]] as this will return both initial
  # fixed and random effects while obj[["par"]] only returns initial fixed
  # effects
  dplyr::mutate(
    initial = c(obj[["env"]][["parameters"]][["p"]], rep(NA_real_, derived_quantity_nrow))
  ) |>
  dplyr::mutate(
    gradient = c(obj[["gr"]](opt[["par"]]), rep(NA_real_, derived_quantity_nrow))
  ) |>
  dplyr::mutate(
    estimated = c(
      rep(TRUE, length(parameter_names)),
      rep(NA, derived_quantity_nrow)
    )
  ) |>
  dplyr::mutate(label_splits = strsplit(label, split = "\\.")) |>
  dplyr::rowwise() |>
  dplyr::mutate(
    module = ifelse(length(label_splits) > 1, label_splits[[1]], NA_character_),
    id = ifelse(length(label_splits) > 1, as.integer(label_splits[[3]]), NA_integer_),
    label = ifelse(length(label_splits) > 1, label_splits[[2]], label),
    index = ifelse(length(label_splits) > 1, as.integer(label_splits[[4]]), NA_integer_),
  ) |>
  dplyr::select(-label_splits) |>
  dplyr::mutate(
    fleet = switch(
      module,
      "selectivity" = {
        # Get the corresponding module ID and filter based on the "id"
        match_module_id <- which(unlist_module_ids[grepl(module, names(unlist_module_ids))] == id)
        strsplit(names(match_module_id), "\\.")[[1]][1]
      },
      "fleet" = names(input[["module_ids"]])[id],
      NA_character_
    )
  ) |>
  dplyr::ungroup()  

updated_estimates <- estimates |>
  dplyr::filter(label == "log_init_naa") |> 
  dplyr::mutate(age = FIMS::get_ages(input[["data"]])) 
  
join <- full_join(estimates, updated_estimates)
join <- union(estimates, updated_estimates)
join |> filter(label == "log_init_naa")

  inner_join(estimates) 

  estimates |> filter(label == "log_init_naa")


updated_estimates |> filter(label == "log_init_naa")
join |> filter(label == "log_init_naa")
  
  

  dplyr::mutate(
    age = dplyr::case_when(
      label == "log_init_naa" ~ as.vector(FIMS::get_ages(input[["data"]])),  # Directly assigns ages for this case
      # label == "NAA" ~ rep(FIMS::get_ages(input[["data"]]), times = FIMS::get_n_years(input[["data"]]) + 1),
      # label == "CNAA" ~ rep(FIMS::get_ages(input[["data"]]), times = FIMS::get_n_years(input[["data"]]) * 2),
      # label == "PCNAA" ~ rep(FIMS::get_ages(input[["data"]]), times = FIMS::get_n_years(input[["data"]]) * 2),
      TRUE ~ age
    )
  )


  dplyr::mutate(
    age = switch(
      label,
      "log_init_naa" = FIMS::get_ages(input[["data"]]),
      "NAA" = rep(FIMS::get_ages(input[["data"]]), times = FIMS::get_n_years(input[["data"]]) + 1),
      "CNAA" = rep(FIMS::get_ages(input[["data"]]), times = FIMS::get_n_years(input[["data"]]) * 2),
      "PCNAA" = rep(FIMS::get_ages(input[["data"]]), times = FIMS::get_n_years(input[["data"]]) * 2),
      NA_real_
    )
  )


data <- tibble(a = 1:4, b = letters[c(1:2, NA, NA)], c = 0.5 + 0:3) |>
  rowwise() |>
  rows_update(tibble(b = "NA", c = 2:3), by = "b", unmatched = "ignore")
