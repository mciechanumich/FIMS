#ifndef FIMS_MODELS_FISHERY_MODEL_BASE_HPP
#define FIMS_MODELS_FISHERY_MODEL_BASE_HPP

#include "../../common/model_object.hpp"


namespace fims_popdy {

    template<typename Type>
    class FisheryModelBase public fims_model_object::FIMSObject<Type> {
        
        public:
            
            virtual void Evaluate() {
                FIMS_WARNING_LOG("Not yet implemented.");
            }
    };

}
#endif
