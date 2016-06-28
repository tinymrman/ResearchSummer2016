#ifndef ITK_LOCALBINARYPATTERNIMAGEFILTER_H
#define ITK_LOCALBINARYPATTERNIMAGEFILTER_H

#include "itkImageToImageFilter.h"

namespace itk
{

template <class TImage>
class LocalBinaryPatternImageFilter : public ImageToImageFilter<TImage,TImage>
{
public:
  typedef LocalBinaryPatternImageFilter       Self;
  typedef ImageToImageFilter<TImage,TImage>   Superclass;
  typedef SmartPointer<Self>                  Pointer;

  itkNewMacro(Self)
  itkTypeMacro(LocalBinaryPatternImageFilter,ImageToImageFilter)

protected:
  LocalBinaryPatternImageFilter() {}
  ~LocalBinaryPatternImageFilter() {}

  virtual void ThreadedGenerateData
  (const typename Superclass::OutputImageRegionType& outputRegionForThread,
  ThreadIdType threadId);

private:
  LocalBinaryPatternImageFilter(const Self&);
  void operator=(const Self&);
};

} //End ITK namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkLocalBinaryPatternImageFilter.txx"
#endif //End ITK_MANUAL_INSTANTIATION

#endif //End ITK_LOCALBINARYPATTERNIMAGEFILTER_H
