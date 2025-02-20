#ifndef FIMS_MODELS_FISHERY_MODEL_BASE_HPP
#define FIMS_MODELS_FISHERY_MODEL_BASE_HPP

#include "../../common/model_object.hpp"


namespace fims_popdy {

    template<typename Type>
    class FisheryModelBase public fims_model_object::FIMSObject<Type> {
        static uint32_t id_g;
        uint32_t id;
    public:

        FisheryModelBase() :
         id(FisheryModelBase::id_g++) {

        }

        virtual void Evaluate() {
            FIMS_WARNING_LOG("Not yet implemented.");
        }

        uint32_t GetId() {
            return this->id;
        }

    };

    template<typename Type>
    uint32_t FisheryModelBase<Type>::id_g = 0;

}
#endif
