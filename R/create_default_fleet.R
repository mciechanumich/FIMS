#' Creates a default input list for a fleet given the data present
#'
#' Creates a default input list for the fleets present in the passed FIMSFrame
#' object dependent upon what type of data is present. This list should be
#' subsequently passed to the `fleets` argument of
#' [create_default_parameters()].
#'
#' @param data A FIMSFrame object.
#' @export
#' @seealso
#' * [create_default_parameters()]
#' * [update_parameters()]
#' @return
#' A named list is returned with the following two elements for each fleet
#' present:
#' \describe{
#'   \item{\code{selectivity}:}{A named list with one entry, \code{form} that
#'     specifies the form of the selectivity module for this fleet.}
#'   \item{\code{data_distribution}:}{A named vector of distributions, one for
#'     each data type present for this fleet.}
#' }
#' @examples
#' data("data1")
#' data_input <- FIMSFrame(data1)
#' create_default_fleet_list(data_input)
#' # Get the list for just one fleet present in the data.
#' create_default_fleet_list(
#'   dplyr::filter(data_input, name == "fleet1") 
#' )
create_default_fleet_list <- function(data) {
  fleets <- get_data(data) |>
    dplyr::pull(name) |>
    unique()
  names(fleets) <- fleets
  purrr::map(
    fleets,
    \(x) create_default_fleet_list_helper(x, data = data)
  )
}

#' A helper function for [create_default_fleet_list()]
#'
#' To make [create_default_fleet_list()] vectorized, we had to create a helper
#' function that can be used internally with {purrr}. This is that helper
#' function, which only works on a string for fleet of length one.
#'
#' @param fleet A string specifying the fleet of interest.
#' @param data A FIMSFrame object.
#' @noRd
create_default_fleet_list_helper <- function(fleet, data) {
  # TODO: return NULL if fleet is not present in data
  relevant_data <- get_data(data) |>
    dplyr::filter(name == fleet)
  data_types <- dplyr::pull(relevant_data, type) |>
    unique()
  distributions <- c()
  if ("landings" %in% data_types | "index" %in% data_types) {
    distributions <- c(
      distributions,
      Index = "DlnormDistribution"
    )
  }
  if ("age" %in% data_types) {
    distributions <- c(
      distributions,
      AgeComp = "DmultinomDistribution"
    )
  }
  if ("length" %in% data_types) {
    distributions <- c(
      distributions,
      LengthComp = "DmultinomDistribution"
    )
  }
  out <- list(
    selectivity = list(
      form = "LogisticSelectivity"
    ),
    data_distribution = distributions
  )
  return(out)
}
