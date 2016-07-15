#ifndef ITK_MINKOWSKIFILTER_H
#define ITK_MINKOWSKIFILTER_H

#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageToImageFilter.h"


namespace itk
{

template <class TInputImage,class TOutputImage>
class Minkowski : public ImageToImageFilter<TInputImage,TOutputImage>
{
public:
  typedef Minkowski                                                      Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>             Superclass;
  typedef SmartPointer<Self>                                          Pointer;

  itkNewMacro(Self)
  itkTypeMacro(Minkowski,ImageToImageFilter)


protected:
  Minkowski() {}
  ~Minkowski() {}

  virtual void GenerateData()
  {
      auto input = this->GetInput();
      this->Update();
      auto size = input->GetLargestPossibleRegion().GetSize();

      auto output = this->GetOutput();
      output->SetRegions(input->GetLargestPossibleRegion());
      output->Allocate();

      typedef ImageLinearConstIteratorWithIndex<TInputImage>    IteratorType;
      typedef ImageLinearIteratorWithIndex<TOutputImage>  OutputIteratorType;

      IteratorType iterator(input,input->GetLargestPossibleRegion());
      OutputIteratorType outputIterator
                                      (output,input->GetLargestPossibleRegion());

      iterator.SetDirection(0);
      outputIterator.SetDirection(0);

      iterator.GoToBegin();

      std::vector<typename Superclass::InputImagePixelType>
                                                       thisSlice(size[0]*size[1]);
      int i = 0;
      thisSlice[i] = iterator.Get();
      i++;
      ++iterator;
      while (!iterator.IsAtEnd())
      {
        while (!iterator.IsAtEndOfLine())
        {
          auto thisPixel = iterator.Get();
          thisSlice[i] = thisPixel;
          ++iterator;
          i++;
        }
        iterator.NextLine();
      }

      std::sort(thisSlice.begin(),thisSlice.end());
      auto onePercentSize = (int) thisSlice.size()/100;

      typename Superclass::InputImagePixelType bottomOnePercent[onePercentSize];
      typename Superclass::InputImagePixelType topOnePercent[onePercentSize];

      for (int j = 0;j < onePercentSize;j++)
      {
        bottomOnePercent[j] = thisSlice[j];
        topOnePercent[(onePercentSize-1)-j] = thisSlice[(thisSlice.size()-1)-j];
      }

      typename Superclass::InputImagePixelType lowerBound;
      typename Superclass::InputImagePixelType upperBound;

      if (onePercentSize % 2 == 0)
      {
        lowerBound = bottomOnePercent[(int)onePercentSize/2];
        upperBound = topOnePercent[(int)onePercentSize/2];

      } else
      {
        lowerBound = (bottomOnePercent[onePercentSize/2]
                       + bottomOnePercent[(onePercentSize/2)-1])/2;

        upperBound = (topOnePercent[onePercentSize/2]
                     + topOnePercent[(onePercentSize/2)-1])/2;
      }
      auto rangeLength = upperBound-lowerBound;

      typename Superclass::OutputImagePixelType zero = 0;
      typename Superclass::OutputImagePixelType one = 1;

      iterator.GoToBegin();
      outputIterator.GoToBegin();
      while (!outputIterator.IsAtEnd())
      {
        while (!outputIterator.IsAtEndOfLine())
        {
          auto inPixel = iterator.Get();

          if (inPixel <= lowerBound)  {outputIterator.Set(zero);
          }
          else if (inPixel >= upperBound)  {outputIterator.Set(one);
          }
          else
          {outputIterator.Set((inPixel-lowerBound)/rangeLength);
          }
          ++iterator;
          ++outputIterator;
        }
        iterator.NextLine();
        outputIterator.NextLine();
      }
  }


private:
  Minkowski(const Self&);
  void operator=(const Self&);

};


} //End ITK namespace

#ifndef ITK_MANUALINSTANTIATION
#include "itkMinkowski.h"
#endif //End ITK_MANUAL_INSTANTIATION

#endif //End ITK_MINKOWSKIFILTER_H
