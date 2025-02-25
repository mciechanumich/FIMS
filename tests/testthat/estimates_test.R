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

estimation_mode = TRUE
map = list()

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

input = parameter_list
get_sd = TRUE
save_sd = TRUE
number_of_loops = 3
optimize = TRUE
number_of_newton_steps = 0
control = list(
eval.max = 10000,
iter.max = 10000,
trace = 0
)
filename = NULL

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

  version = utils::packageVersion("FIMS")

# What we aspire the estimate table to look like
  estimates_outline <- dplyr::tibble(
    label = character(),
    fleet = character(),
    age = numeric(),
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


partial_match <- lapply(input$module_ids, function(x) grep("selectivity", names(x)))

fleet_name <- names(input)[sapply(input, function(x) {
          if (is.data.frame(x)) {
            # Match 'selectivity' column value to 'id'
            any(x$selectivity == id)  # Check if 'selectivity' matches 'id'
          } else {
            FALSE
          }
        })]