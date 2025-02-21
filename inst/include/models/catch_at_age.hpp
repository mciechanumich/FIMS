#ifndef FIMS_MODELS_CATACH_AT_AGE_HPP
#define FIMS_MODELS_CATACH_AT_AGE_HPP

#include <regex>

#include "fishery_model_base.hpp"
#include "../population_dynamics/population/population.hpp"

namespace fims_popdy {

    template<typename Type>
    class CatchAtAge : public FisheryModelBase<Type> {
    public:


        std::set<uint32_t> population_ids;

    public:
        std::vector<std::shared_ptr<fims_popdy::Population> > populations;

        CatchAtAge() : FisheryModelBase<Type>() {
        }

        virtual void Intialize() {
            for (size_t i = 0; i < this->populations.size(); i++) {
                this->populations[i]->derived_quantities["mortality_F"] =
                        fims::Vector<Type>(this->populations[i]->nyears *
                        this->populations[i]->nages);
                this->populations[i]->derived_quantities["mortality_Z"] =
                        fims::Vector<Type>(this->populations[i]->nyears *
                        this->populations[i]->nages);
                this->populations[i]->derived_quantities["weight_at_age"] =
                        fims::Vector<Type>(this->populations[i]->nyears *
                        this->populations[i]->nages);

                this->populations[i]->derived_quantities["numbers_at_age"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1) *
                        this->populations[i]->nages);
                this->populations[i]->derived_quantities["unfished_numbers_at_age"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1) *
                        this->populations[i]->nages);
                this->populations[i]->derived_quantities["biomass"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1));
                this->populations[i]->derived_quantities["spawning_biomass"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1));
                this->populations[i]->derived_quantities["unfished_biomass"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1));
                this->populations[i]->derived_quantities["unfished_spawning_biomass"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1));
                this->populations[i]->derived_quantities["proportion_mature_at_age"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1) *
                        this->populations[i]->nages);
                this->populations[i]->derived_quantities["expected_catch"] =
                        fims::Vector<Type>(this->populations[i]->nyears *
                        this->populations[i]->nfleets);
                this->populations[i]->derived_quantities["expected_recruitment"] =
                        fims::Vector<Type>((this->populations[i]->nyears + 1));
            }
        }

        void Prepare() {

        }

        void AddPopulation(uint32_t id) {
            this->population_ids.insert(id);
        }

        void CalculateInitialNumbersAA(size_t i_age_year, size_t a) {
            for (size_t p = 0; p < this->populations.size(); p++) {
                this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] =
                        fims_math::exp(this->populations[p]->derived_quantities["log_init_naa"][a]);
            }
        }

        void CalculateNumbersAA(
                size_t i_age_year,
                size_t i_agem1_yearm1,
                size_t age) {
            // using Z from previous age/year
            for (size_t p = 0; p < this->populations.size(); p++) {
                this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] =
                        this->populations[p]->derived_quantities["numbers_at_age"][i_agem1_yearm1] *
                        (fims_math::exp(-this->populations[p]->derived_quantities["mortality_Z"][i_agem1_yearm1]));

                // Plus group calculation
                if (age == (this->nages - 1)) {
                    this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] =
                            this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] +
                            this->populations[p]->derived_quantities["numbers_at_age"][i_agem1_yearm1 + 1] *
                            (fims_math::exp(-this->populations[p]->derived_quantities["mortality_Z"][i_agem1_yearm1 + 1]));
                }
            }
        }

        void CalculateUnfishedNumbersAA(size_t i_age_year,
                size_t i_agem1_yearm1, size_t age) {

            for (size_t p = 0; p < this->populations.size(); p++) {

                // using M from previous age/year
                this->populations[p]->derived_quantities["unfished_numbers_at_age"][i_age_year] =
                        this->populations[p]->derived_quantities["unfished_numbers_at_age"][i_agem1_yearm1] *
                        (fims_math::exp(-this->populations[p]->derived_quantities["M"][i_agem1_yearm1]));

                // Plus group calculation
                if (age == (this->nages - 1)) {
                    this->populations[p]->derived_quantities["unfished_numbers_at_age"][i_age_year] =
                            this->populations[p]->derived_quantities["unfished_numbers_at_age"][i_age_year] +
                            this->populations[p]->derived_quantities["unfished_numbers_at_age"][i_agem1_yearm1 + 1] *
                            (fims_math::exp(-this->populations[p]->derived_quantities["M"][i_agem1_yearm1 + 1]));
                }
            }
        }

        void CalculateMortality(
                size_t i_age_year, size_t year, size_t age) {

            for (size_t p = 0; p < this->populations.size(); p++) {
                for (size_t fleet_ = 0; fleet_ < this->populations[p]->nfleets; fleet_++) {
                    if (this->populations[p]->fleets[fleet_]->is_survey == false) {
                        this->populations[p]->derived_quantities["mortality_F"][i_age_year] +=
                                this->populations[p]->fleets[fleet_]->Fmort[year] *
                                // evaluate is a member function of the selectivity class
                                this->populations[p]->fleets[[fleet_]->selectivity->evaluate(this->populations[p]->ages[age]);
                    }
                }
            }
        }

        void CalculateBiomass(size_t i_age_year, size_t year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {
                this->populations[p]->derived_quantities["biomass"][year] +=
                        this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] *
                        this->populations[p]->weight_at_age[age];
            }
        }

        void CalculateUnfishedBiomass(size_t i_age_year, size_t year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {
                this->populations[p]->derived_quantities["unfished_biomass"][year] +=
                        this->populations[p]->derived_quantities["unfished_numbers_at_age"][i_age_year] *
                        this->populations[p]->weight_at_age[age];
            }

        }

        void CalculateSpawningBiomass(size_t i_age_year, size_t year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {
                this->populations[p]->derived_quantities["spawning_biomass"][year] +=
                        this->populations[p]->proportion_female[age] *
                        this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] *
                        this->populations[p]->proportion_mature_at_age[i_age_year] *
                        this->populations[p]->weight_at_age[age];
            }

        }

        void CalculateUnfishedSpawningBiomass(size_t i_age_year, size_t year,
                size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {
                this->populations[p]->derived_quantities["unfished_spawning_biomass"][year] +=
                        this->populations[p]->proportion_female[age] *
                        this->populations[p]->derived_quantities["unfished_numbers_at_age"][i_age_year] *
                        this->populations[p]->proportion_mature_at_age[i_age_year] *
                        this->populations[p]->weight_at_age[age];
            }
        }

        std::vector<Type> CalculateSBPR0() {

            std::vector<Type> phis;
            for (size_t p = 0; p < this->populations.size(); p++) {

                std::vector<Type> numbers_spr(this->nages, 1.0);
                Type phi_0 = 0.0;
                phi_0 += numbers_spr[0] * this->populations[p]->proportion_female[0] *
                        this->populations[p]->proportion_mature_at_age[0] *
                        this->populations[p]->growth->evaluate(ages[0]);
                for (size_t a = 1; a < (this->populations[p]->nages - 1); a++) {
                    numbers_spr[a] = numbers_spr[a - 1] * fims_math::exp(-this->populations[p]->M[a]);
                    phi_0 += numbers_spr[a] * this->proportion_female[a] *
                            this->populations[p]->proportion_mature_at_age[a] *
                            this->populations[p]->growth->evaluate(ages[a]);
                }

                numbers_spr[this->nages - 1] =
                        (numbers_spr[nages - 2] * fims_math::exp(-this->populations[p]->M[nages - 2])) /
                        (1 - fims_math::exp(-this->populations[p]->M[this->populations[p]->nages - 1]));
                phi_0 += numbers_spr[this->populations[p]->nages - 1] *
                        this->populations[p]->proportion_female[this->nages - 1] *
                        this->populations[p]->proportion_mature_at_age[this->nages - 1] *
                        this->populations[p]->growth->evaluate(ages[this->nages - 1]);
                phis.push_back(phi_0);
            }

            return phis;
        }

        void CalculateRecruitment(size_t i_age_year, size_t year, size_t i_dev) {

            std::vector<Type> phi0 = CalculateSBPR0();

            for (size_t p = 0; p < this->populations.size(); p++) {


                if (i_dev == this->populations[p]->nyears) {
                    this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] =
                            this->populations[p]->recruitment->evaluate(
                            this->populations[p]->derived_quantities["spawning_biomass"][year - 1], phi0);
                    /*the final year of the time series has no data to inform recruitment
                    devs, so this value is set to the mean recruitment.*/
                } else {
                    this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] =
                            this->recruitment->evaluate(
                            this->populations[p]->derived_quantities["spawning_biomass"][year - 1], phi0) *
                            /*the log_recruit_dev vector does not include a value for year == 0
                            and is of length nyears - 1 where the first position of the vector
                            corresponds to the second year of the time series.*/
                            fims_math::exp(this->populations[p]->recruitment->log_recruit_devs[i_dev - 1]);
                    this->populations[p]->derived_quantities["expected_recruitment"][year] =
                            this->populations[p]->derived_quantities["numbers_at_age"][i_age_year];
                }



            }
        }

        void CalculateCatch(size_t year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {

                for (size_t fleet_ = 0; fleet_ < this->populations[p]->nfleets; fleet_++) {
                    if (this->populations[p]->fleets[fleet_]->is_survey == false) {
                        size_t index_yf = year * this->populations[p]->nfleets +
                                fleet_; // index by fleet and years to dimension fold
                        size_t i_age_year = year * this->populations[p]->nages + age;

                        this->populations[p]->derived_quantities["expected_catch"][index_yf] +=
                                this->populations[p]->fleets[fleet_]->catch_weight_at_age[i_age_year];

                        this->populations[p]->fleets[fleet_]->expected_catch[year] +=
                                this->populations[p]->fleets[fleet_]->catch_weight_at_age[i_age_year];
                    }
                }

            }
        }

        void CalculateIndex(size_t i_age_year, size_t year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {

                for (size_t fleet_ = 0; fleet_ < this->populations[p]->nfleets; fleet_++) {
                    Type index_;
                    // I = qN (N is total numbers), I is an index in numbers
                    if (this->populations[p]->fleets[fleet_]->is_survey == false) {
                        index_ = this->populations[p]->fleets[fleet_]->catch_numbers_at_age[i_age_year] *
                                this->populations[p]->derived_quantities["weight_at_age"][age];
                    } else {
                        index_ = this->populations[p]->fleets[fleet_]->q.get_force_scalar(year) *
                                this->populations[p]->fleets[fleet_]->selectivity->evaluate(ages[age]) *
                                this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] *
                                this->populations[p]->derived_quantities["weight_at_age"][age]; // this->weight_at_age[age];
                    }
                    this->populations[p]->fleets[fleet_]->expected_index[year] += index_;
                }
            }
        }

        void CalculateCatchNumbersAA(size_t i_age_year, size_t year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {

                for (size_t fleet_ = 0; fleet_ < this->populations[p]->nfleets; fleet_++) {
                    // make an intermediate value in order to set multiple members (of
                    // current and fleet objects) to that value.
                    Type catch_; // catch_ is used to avoid using the c++ keyword catch
                    // Baranov Catch Equation
                    if (this->populations[p]->fleets[fleet_]->is_survey == false) {
                        catch_ = (this->populations[p]->fleets[fleet_]->Fmort[year] *
                                this->populations[p]->fleets[fleet_]->selectivity->evaluate(ages[age])) /
                                this->populations[p]->derived_quantities["mortality_Z"][i_age_year] *
                                this->populations[p]->derived_quantities["numbers_at_age"][i_age_year] *
                                (1 - fims_math::exp(-(this->populations[p]->derived_quantities["mortality_Z"][i_age_year])));
                    } else {
                        catch_ = (this->populations[p]->fleets[fleet_]->selectivity->evaluate(ages[age])) *
                                this->populations[p]->derived_quantities["numbers_at_age"][i_age_year];
                    }

                    // this->catch_numbers_at_age[i_age_yearf] += catch_;
                    // catch_numbers_at_age for the fleet module has different
                    // dimensions (year/age, not year/fleet/age)
                    this->populations[p]->fleets[fleet_]->catch_numbers_at_age[i_age_year] += catch_;
                }

            }
        }

        void CalculateCatchWeightAA(size_t year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {

                int i_age_year = year * this->populations[p]->nages + age;
                for (size_t fleet_ = 0; fleet_ < this->populations[p]->nfleets; fleet_++) {

                    this->populations[p]->fleets[fleet_]->catch_weight_at_age[i_age_year] =
                            this->populations[p]->fleets[fleet_]->catch_numbers_at_age[i_age_year] *
                            this->populations[p]->derived_quantities["weight_at_age"][age];

                }

            }
        }

        void CalculateMaturityAA(size_t i_age_year, size_t age) {
            for (size_t p = 0; p < this->populations.size(); p++) {
                 this->populations[p]->proportion_mature_at_age[i_age_year] =
                         this->populations[p]->maturity->evaluate( this->populations[p]->ages[age]);
            }
        }

        virtual void Evaluate() {
            for (size_t i = 0; i < this->populations.size(); i++) {
                // this->populations[i]->Evaluate();
            }
        }


    };


}



#endif
