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

        void CalculateInitialNumbersAA( size_t i_age_year, size_t a) { 
            for(size_t p =0; p < this->populations.size(); p++){
                this->populations[i]->derived_quantities["numbers_at_age"][i_age_year] =
                        fims_math::exp(this->populations[i]->derived_quantities["log_init_naa"][a]); 
            }
        }

        void CalculateNumbersAA() {

        }

        void CalculateUnfishedNumbersAA() {

        }

        void CalculateMortality() {

        }

        void CalculateBiomass() {

        }

        void CalculateUnfishedBiomass() {

        }

        void CalculateSpawningBiomass() {

        }

        void CalculateUnfishedSpawningBiomass() {

        }


        //matthew: changed to void

        void CalculateSBPR0() {

        }

        void CalculateRecruitment() {

        }

        void CalculateCatch() {

        }

        void CalculateIndex() {

        }

        void CalculateCatchNumbersAA() {

        }

        void CalculateCatchWeightAA() {

        }

        void CalculateMaturityAA() {

        }

        virtual void Evaluate() {
            for (size_t i = 0; i < this->populations.size(); i++) {
                // this->populations[i]->Evaluate();
            }
        }
        
        
    };


}



#endif
