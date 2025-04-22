
#include <cstdint>
#include <ATen/native/TensorIterator.h>
#include <ATen/native/cuda/MemoryAccess.cuh>

namespace at::native {

template<int alignment>
inline bool fast_gather_kernel_eligible(const TensorIterator& iter, char * const out_ptr, char * const in_ptr, const size_t index_stride, const size_t element_size) {
  using at::native::memory::get_alignment;
  const auto index_element_size = iter.element_size(2);
  //TensorIterator strides and sizes are ordered fastest moving to slowest moving,
  //in contrast to regular sizes
  // we need contiguous source and dst slices and aligned pointers and strides and slice size to do vectorized loads
  // also we need idx to be expanded in the last dimension so we can copy entire slices

  return iter.ndim() == 2 && iter.strides(2)[0]==0 && iter.strides(2)[1]==index_element_size && static_cast<size_t>(iter.strides(0)[0])==element_size &&
          static_cast<size_t>(iter.strides(1)[0])==element_size && get_alignment(out_ptr) == alignment && get_alignment(in_ptr) == alignment &&
          get_alignment(static_cast<size_t>(iter.shape()[0] * element_size)) == alignment &&
          get_alignment(static_cast<size_t>(index_stride * element_size)) == alignment &&
          get_alignment(static_cast<size_t>(iter.strides(0)[1])) == alignment;
}

template <int64_t Alignment, typename index_t>
void vectorized_gather_kernel_launch(char * out, char * inp, index_t * idx, int num_ind,
                                     int64_t slice_size_in_bytes, int64_t ind_dim_size, int64_t inp_stride_bytes, int64_t out_stride_bytes,
                                     bool allow_neg_indices=false);


}
