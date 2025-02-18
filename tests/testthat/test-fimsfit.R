test_that("FIMSFit is not returned from a wrong input", {
  expect_false(is.FIMSFit("not_a_FIMSFit"))
})

test_that("FIMSFit() stops for missing input", {
  expect_error(
    FIMSFit(
      obj = list(
        par = NA, fn = NA, gr = NA, he = NA, hessian = NA, method = NA,
        retape = NA,
        env = list("last.par.best" = 1, "parList" = function(x) {
          list("p" = TRUE)
        }),
        report = function(x) {
          list(TRUE)
        }, simulate = NA
      )
    ),
    "missing, with no default"
  )
})

test_that("FIMSFit() estimates tibble has correct column names", {
  # Load the test data from an RDS file containing the fitted model estimates
  estimates <- readRDS(test_path("fixtures", "estimates_age_length_comp.RDS"))

  # Retrieve the column names of the estimates tibble
  estimates_colnames <- colnames(estimates)

  # Define the expected column names for the estimates tibble
  expected_colnames <- c(
    "label", "fleet", "age", "time", "initial", "estimate", "uncertainty",
    "likelihood", "gradient", "estimated"
  )

  # Check that the column names in the estimates tibble match the expected column names
  expect_equal(estimates_colnames, expected_colnames)
})