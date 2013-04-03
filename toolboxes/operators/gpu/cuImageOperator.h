#pragma once

#include "cuNDArray_operators.h"
#include "cuNDArray_elemwise.h"
#include "cuNDArray_blas.h"
#include "complext.h"
#include "imageOperator.h"

#include <cmath>
#include <algorithm>

namespace Gadgetron{

  template <class T> class cuImageOperator : public imageOperator< cuNDArray<typename realType<T>::Type >, cuNDArray<T> >
  {
  public:

    cuImageOperator() : imageOperator< cuNDArray<typename realType<T>::Type >, cuNDArray<T> >() {}
    virtual ~cuImageOperator() {}    

    typedef typename imageOperator< cuNDArray<typename realType<T>::Type>, cuNDArray<T> >::REAL REAL;

    virtual boost::shared_ptr< linearOperator< cuNDArray<T> > > clone() {
      return linearOperator< cuNDArray<T> >::clone(this);
    }

  protected:

    // Estimate offset to the regularization image
    virtual REAL estimate_offset()
    {
      // Estimation based on simple histogram analysis:
      // Returns an estimation of the "average" intensity of the 'sigma' proportion of the image with the smallest intensities.
      //
      
      // This simple code is fast enough (<.5 ms on a 192x192 image) that we can just copy the hoImageOperators host code
      //

      const unsigned int granularity = 50000; 
      std::vector<unsigned int> histogram(granularity,0);
      REAL max_value = this->image_->at(amax(this->image_.get()));
      boost::shared_ptr<hoNDArray<REAL> > tmp = this->image_->to_host();
      REAL *d = tmp->get_data_ptr();
      
      for( unsigned int i=0; i<this->image_->get_number_of_elements(); i++) {
	unsigned int bin = std::min(static_cast<unsigned int>(std::floor((d[i]/max_value)*granularity)), granularity-1);
	histogram[bin]++;
      }
      
      //Find 1th percentile
      //
      
      unsigned int cumsum = 0, counter = 0;
      while (cumsum < (unsigned int)(REAL(0.01)*this->image_->get_number_of_elements())) {
	cumsum += histogram[counter++];
      }      

      return  REAL(counter+1)*max_value/granularity;
    }
  };
}