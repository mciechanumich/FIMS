library(testthat)
test_that("Population input settings work as expected", {
  population <- methods::new(Population)
  nyears <- 10
  nages <- 10
  population$log_M <- methods::new(ParameterVector, rep(-1, nyears * nages), nyears * nages)
  population$log_init_naa <- methods::new(ParameterVector, log(rep(1, nages)), nages)
  population$log_init_naa$set_all_estimable(TRUE)
  population$nages$set(nages)
  population$ages$fromR(1:nages)
  population$nfleets$set(2)
  population$nseasons$set(1)
  population$nyears$set(nyears)

  expect_equal(population$get_id(), 1)
  for (i in 1:(nyears * nages)) {
    expect_equal(population$log_M[i]$value, -1)
    expect_false(population$log_M[i]$estimated)
  }
  for (i in 1:nyears) {
    expect_equal(population$log_init_naa[i]$value, 0)
    expect_true(population$log_init_naa[i]$estimated)
  }

  clear()
})
