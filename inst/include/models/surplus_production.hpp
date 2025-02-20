#ifndef FIMS_MODELS_SURPLUS_PRODUCTION_HPP
#define FIMS_MODELS_SURPLUS_PRODUCTION_HPP

#include <set>
#include "fishery_model_base.hpp"

namespace fims_popdy {

    template<typename Type>
    class SurplusProduction : public FisheryModelBase<Type> {
        
        std::set<uint32_t> population_ids;
        
    public:

        virtual void Evaluate() {
        }
        
        
        void AddPopulation(uint32_t id){
            this->population_ids.insert(id);
        }
    };


}


#endif
