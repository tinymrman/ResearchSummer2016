#ifndef ITK_LOCALBINARYPATTERNIMAGEFILTER_TXX
#define ITK_LOCALBINARYPATTERNIMAGEFILTER_TXX

#include "itkLocalBinaryPatternImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageSliceIteratorWithIndex.h"
#include "itkImage.h"

namespace itk
{

template <class TImage>
void LocalBinaryPatternImageFilter<TImage>::
ThreadedGenerateData
(const typename Superclass::OutputImageRegionType& outputRegionForThread,
ThreadIdType threadId)
{
  auto input = this->GetInput();

  auto output = this->GetOutput();
  output->SetRegions(input->GetLargestPossibleRegion());
  output->Allocate();

  typedef itk::ConstNeighborhoodIterator<TImage>     NeighborhoodIteratorType;
  typedef itk::ImageSliceConstIteratorWithIndex<TImage>     InputIteratorType;
  typedef itk::ImageSliceIteratorWithIndex<TImage>         OutputIteratorType;

  InputIteratorType inputIterator(input,outputRegionForThread);
  OutputIteratorType outputIterator(output,outputRegionForThread);

  inputIterator.SetFirstDirection(0);
  inputIterator.SetSecondDirection(1);
  outputIterator.SetFirstDirection(0);
  outputIterator.SetSecondDirection(1);

  typename NeighborhoodIteratorType::RadiusType radius;
  radius.Fill(1);
  NeighborhoodIteratorType
          neighborhoodIterator(radius,input,outputRegionForThread);

  typename NeighborhoodIteratorType::OffsetType offset0 = {{-1,-1,0}};
  typename NeighborhoodIteratorType::OffsetType offset1 = {{0,-1,0}};
  typename NeighborhoodIteratorType::OffsetType offset2 = {{1,-1,0}};
  typename NeighborhoodIteratorType::OffsetType offset3 = {{1,0,0}};
  typename NeighborhoodIteratorType::OffsetType offset4 = {{1,1,0}};
  typename NeighborhoodIteratorType::OffsetType offset5 = {{0,1,0}};
  typename NeighborhoodIteratorType::OffsetType offset6 = {{-1,1,0}};
  typename NeighborhoodIteratorType::OffsetType offset7 = {{-1,0,0}};

  inputIterator.GoToBegin();
  outputIterator.GoToBegin();
  while (!inputIterator.IsAtEnd())
  {
    while(!inputIterator.IsAtEndOfSlice())
    {
      while(!inputIterator.IsAtEndOfLine())
      {
        neighborhoodIterator.SetLocation(inputIterator.GetIndex());
        typename Superclass::OutputImagePixelType
                result = 0;
        typename Superclass::InputImagePixelType
                center = neighborhoodIterator.GetCenterPixel();

        if (neighborhoodIterator.GetPixel(offset0) >= center)
            result += pow(2,0);
        if (neighborhoodIterator.GetPixel(offset1) >= center)
            result += pow(2,1);
        if (neighborhoodIterator.GetPixel(offset2) >= center)
            result += pow(2,2);
        if (neighborhoodIterator.GetPixel(offset3) >= center)
            result += pow(2,3);
        if (neighborhoodIterator.GetPixel(offset4) >= center)
            result += pow(2,4);
        if (neighborhoodIterator.GetPixel(offset5) >= center)
            result += pow(2,5);
        if (neighborhoodIterator.GetPixel(offset6) >= center)
            result += pow(2,6);
        if (neighborhoodIterator.GetPixel(offset7) >= center)
            result += pow(2,7);

        outputIterator.Set(result);

        ++inputIterator;
        ++outputIterator;
      }
      inputIterator.NextLine();
      outputIterator.NextLine();
    }
    inputIterator.NextSlice();
    outputIterator.NextSlice();
  }

}


} //End ITK namespace
#endif
