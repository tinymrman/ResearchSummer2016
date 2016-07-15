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
GenerateData()
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

  InputIteratorType inputIterator(input,input->GetLargestPossibleRegion());
  OutputIteratorType outputIterator(output,input->GetLargestPossibleRegion());

  inputIterator.SetFirstDirection(1);
  inputIterator.SetSecondDirection(0);
  outputIterator.SetFirstDirection(0);
  outputIterator.SetSecondDirection(1);

  inputIterator.GoToBegin();
  outputIterator.GoToBegin();
  int verticalHalfwayOffset = size[1]/2;

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
        firstOutput[i+verticalHalfwayOffset][j] = haarDifference;
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

  this->Update();
  if (this->m_ThirdDimension == 1)
  {
    std::cout << "\nThe 3-dimensional filter was applied\n";

    typedef itk::ImageSliceConstIteratorWithIndex<OutputTImage>
                                                     ConstOutputIteratorType;

    ConstOutputIteratorType thisInput(this->GetOutput(),output->GetLargestPossibleRegion());
    OutputIteratorType      thisOutput(this->GetOutput(),output->GetLargestPossibleRegion());

    thisInput.SetFirstDirection(2);
    thisInput.SetSecondDirection(1);
    thisOutput.SetFirstDirection(2);
    thisOutput.SetSecondDirection(1);

    thisInput.GoToBegin();
    thisOutput.GoToBegin();

    typename Superclass::OutputImagePixelType    secondHalf[size[2]/2];

    typename Superclass::OutputImagePixelType    firstPixel;
    typename Superclass::OutputImagePixelType    secondPixel;
    typename Superclass::OutputImagePixelType    haarAverage;
    typename Superclass::OutputImagePixelType    haarDifference;

    while (!thisOutput.IsAtEnd())
    {
      while (!thisOutput.IsAtEndOfSlice())
      {
        int p = 0;
        while(!thisInput.IsAtEndOfLine())
        {
          firstPixel = thisInput.Get();
          ++thisInput;
          secondPixel = thisInput.Get();
          ++thisInput;
          haarAverage = (0.5)*(firstPixel+secondPixel);
          haarDifference = (0.5)*(firstPixel-secondPixel);
          thisOutput.Set(haarAverage);
          ++thisOutput;
          secondHalf[p] = haarDifference;
          p++;
        }
        int q = 0;
        while(!thisOutput.IsAtEndOfLine())
        {
          thisOutput.Set(secondHalf[q]);
          ++thisOutput;
          q++;
        }
        thisOutput.NextLine();
        thisInput.NextLine();
      }
      thisOutput.NextSlice();
      thisInput.NextSlice();
    }
  } else
    {std::cout << "\nThe 2-dimensional filter was applied\n";}
}

} //End ITK namespace

#endif
