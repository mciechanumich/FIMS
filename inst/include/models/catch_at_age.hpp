#ifndef FIMS_MODELS_CATACH_AT_AGE_HPP
#define FIMS_MODELS_CATACH_AT_AGE_HPP

#include "fishery_model_base.hpp"
#include "../population_dynamics/population/population.hpp"

namespace fims_popdy {

    template<typename Type>
    class CatchAtAge : public FisheryModelBase<Type> {
    public:


        std::set<uint32_t> population_ids;

    public:
        std::vector<std::shared_ptr<fims_popdy::Population> > populations;

        virtual void Intialize() {
            for (size_t i = 0; i < this->populations.size(); i++) {
                this->populations[i]->derived_quantities["weight_at_age"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["numbers_at_age"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["unfished_numbers_at_age"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["biomass"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["spawning_biomass"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["unfished_biomass"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["unfished_spawning_biomass"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["proportion_mature_at_age"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["expected_catch"] = fims::Vector<Type>();
                this->populations[i]->derived_quantities["expected_recruitment"] = fims::Vector<Type>();
            }
        }

        virtual void Evaluate() {
            for (size_t i = 0; i < this->populations.size(); i++) {
                // this->populations[i]->Evaluate();
            }
        }

        void AddPopulation(uint32_t id) {
            this->population_ids.insert(id);
        }
    };


}



#endif
