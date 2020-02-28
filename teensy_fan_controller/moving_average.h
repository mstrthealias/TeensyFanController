//
// Created by jd on 11/26/2019.
//

#ifndef TFC_MOVINGAVERAGE_H
#define TFC_MOVINGAVERAGE_H

#include <Arduino.h>


/**
   Source: https://stackoverflow.com/a/10990893/1345237
*/
template<typename T, typename Total, uint16_t N>
class Moving_Average {
  public:
    void operator()(T sample) {
      if (num_samples_ < N) {
        samples_[num_samples_++] = sample;
        total_ += sample;
      }
      else {
        T &oldest = samples_[num_samples_++ % N];
        total_ += sample - oldest;
        oldest = sample;
      }
    }

    operator double() const {
      return total_ / min(num_samples_, N);
    }

  private:
    T samples_[N];
    uint16_t num_samples_{0};
    Total total_{0};
};

#endif //TFC_MOVINGAVERAGE_H
