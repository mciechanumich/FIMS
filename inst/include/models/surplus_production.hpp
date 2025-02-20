#ifndef FIMS_MODELS_SURPLUS_PRODUCTION_HPP
#define FIMS_MODELS_SURPLUS_PRODUCTION_HPP


#include "fishery_model_base.hpp"

namespace fims_popdy {

    template<typename Type>
    class SurplusProduction : public FisheryModelBase<Type> {
    public:

        virtual void Evaluate() {
        }
    };


}


#endif
