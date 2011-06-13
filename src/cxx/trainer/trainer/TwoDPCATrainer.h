#ifndef TORCH5SPRO_TRAINER_TWODPCA_TRAINER_H
#define TORCH5SPRO_TRAINER_TWODPCA_TRAINER_H

#include "Trainer.h"
#include "core/logging.h"
#include "database/Arrayset.h"
#include "machine/TwoDPCAMachine.h"
#include "machine/ImageSample.h"
#include <algorithm>
#include <utility>
#include <vector>

namespace Torch 
{
  namespace trainer 
  {
  
    class TwoDPCATrainer : virtual public Trainer<Torch::machine::TwoDPCAMachine, Torch::database::Arrayset>
    {
      public:
        TwoDPCATrainer() {}
        virtual ~TwoDPCATrainer() {}
  
        void train(Torch::machine::TwoDPCAMachine& machine, const Torch::database::Arrayset& data); 

      protected:

    };

  }
}

#endif /* TORCH5SPRO_TRAINER_TWODPCA_TRAINER_H */