#ifndef FIMS_MODELS_SURPLUS_PRODUCTION_HPP
#define FIMS_MODELS_SURPLUS_PRODUCTION_HPP

#include <set>
#include <regex>
#include "fishery_model_base.hpp"

namespace fims_popdy {

    template<typename Type>
    class SurplusProduction : public FisheryModelBase<Type> {
        std::set<uint32_t> population_ids;

    public:
        std::vector<std::shared_ptr<fims_popdy::Population> > populations;

        virtual void Evaluate() {
            for(size_t i =0; i < this->populations.size(); i++){
               // this->populations[i]->Evaluate();
            }
        }

        void AddPopulation(uint32_t id) {
            this->population_ids.insert(id);
        }
    };


}


#endif
