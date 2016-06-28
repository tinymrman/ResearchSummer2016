#ifndef ITK_HAARWAVELETIMAGEFILTER_TXX
#define ITK_HAARWAVELETIMAGEFILTER_TXX

#include "itkHaarWaveletImageFilter.h"
#include "itkImage.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageSliceIteratorWithIndex.h"

namespace itk
{

template <class InputTImage,class OutputTImage>
void HaarWaveletImageFilter<InputTImage,OutputTImage>::
ThreadedGenerateData
(const typename Superclass::OutputImageRegionType& outputRegionForThread,
ThreadIdType threadId)
{
  auto input = this->GetInput();
  auto size = input->GetLargestPossibleRegion().GetSize();

  auto output = this->GetOutput();
  output->SetRegions(input->GetLargestPossibleRegion());
  output->Allocate();

  typedef itk::
          ImageSliceConstIteratorWithIndex<InputTImage>  InputIteratorType;
  typedef itk::
          ImageSliceIteratorWithIndex<OutputTImage>      OutputIteratorType;

  InputIteratorType inputIterator(input,outputRegionForThread);
  OutputIteratorType outputIterator(output,outputRegionForThread);

  inputIterator.SetFirstDirection(1);
  inputIterator.SetSecondDirection(0);
  outputIterator.SetFirstDirection(0);
  outputIterator.SetSecondDirection(1);

  inputIterator.GoToBegin();
  outputIterator.GoToBegin();
  int halfwayOffset = size[1]/2;

  while (!inputIterator.IsAtEnd())
  {
    typename Superclass::OutputImagePixelType firstOutput[size[1]][size[0]];
    typename Superclass::OutputImagePixelType haarAverage;
    typename Superclass::OutputImagePixelType haarDifference;
    typename Superclass::InputImagePixelType  firstPixelIn;
    typename Superclass::InputImagePixelType  secondPixelIn;
    int j = 0;
    int i = 0;
    while(!inputIterator.IsAtEndOfSlice())
    {
      i = 0;
      while (!inputIterator.IsAtEndOfLine())
      {
        firstPixelIn = inputIterator.Get();
        ++inputIterator;
        secondPixelIn = inputIterator.Get();
        ++inputIterator;
        haarAverage = (0.5)*(firstPixelIn+secondPixelIn);
        haarDifference = (0.5)*(firstPixelIn-secondPixelIn);
        firstOutput[i][j] = haarAverage;
        firstOutput[i+halfwayOffset][j] = haarDifference;
        i++;
      }
      j++;
      inputIterator.NextLine();
    }
    typename Superclass::OutputImagePixelType    firstPixelOut;
    typename Superclass::OutputImagePixelType    secondPixelOut;

    i = 0;
    while(!outputIterator.IsAtEndOfSlice())
    {
      j = 0;
      typename Superclass::OutputImagePixelType secondHalf[size[0]/2];
      int k = 0;
      while (j < size[0])
      {
        firstPixelOut = firstOutput[i][j];
        j++;
        secondPixelOut = firstOutput[i][j];
        j++;
        haarAverage = (0.5)*(firstPixelOut+secondPixelOut);
        outputIterator.Set(haarAverage);
        ++outputIterator;
        haarDifference = (0.5)*(firstPixelOut-secondPixelOut);
        secondHalf[k] = haarDifference;
        k++;
      }
      for(int m = 0;m < k;m++)
      {
        outputIterator.Set(secondHalf[m]);
        ++outputIterator;
      }
      i++;
      outputIterator.NextLine();
    }
    inputIterator.NextSlice();
    outputIterator.NextSlice();
  }
}

} //End ITK namespace

#endif
