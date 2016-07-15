#ifndef ITK_HAARWAVELETIMAGEFILTER_H
#define ITK_HAARWAVELETIMAGEFILTER_H

#include "itkImageToImageFilter.h"
#include "itkImageSliceConstIteratorWithIndex.h"

namespace itk
{

template <class InputTImage,class OutputTImage>
class HaarWaveletImageFilter :
public ImageToImageFilter<InputTImage,OutputTImage>
{
public:
  typedef HaarWaveletImageFilter                           Self;
  typedef ImageToImageFilter<InputTImage,OutputTImage>     Superclass;
  typedef SmartPointer<Self>                               Pointer;

  itkNewMacro(Self)
  itkTypeMacro(HaarWaveletImageFilter,ImageToImageFilter)
  itkSetMacro(ThirdDimension,int)
  itkGetMacro(ThirdDimension,int)


protected:
  HaarWaveletImageFilter() {}
  ~HaarWaveletImageFilter() {}

  virtual void GenerateData();


private:
  HaarWaveletImageFilter(const Self&);
  void operator=(const Self&);

  int m_ThirdDimension;
};

} //End ITK namespace

#ifndef ITK_MANUAL_INSTATIATION
#include "itkHaarWaveletImageFilter.txx"
#endif //End ITK_MANUAL_INSTANTIATION

#endif //END ITK_HAARWAVELETIMAGEFILTER_H
