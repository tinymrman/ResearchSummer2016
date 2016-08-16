#ifndef ITK_GRAYLEVELDIFFERENCECALCULATOR_H
#define ITK_GRAYLEVELDIFFERENCECALCULATOR_H

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkImage.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkHistogram.h"

namespace itk
{

template <class TImage,class TInternalImage>
class GrayLevelDifferenceCalculator :
public Object
{
public:
  typedef GrayLevelDifferenceCalculator                                  Self;
  typedef Object                                                   Superclass;
  typedef SmartPointer<Self>                                          Pointer;
  typedef TImage                                                    ImageType;
  typedef TInternalImage                                    InternalImageType;

  itkNewMacro(Self)
  itkTypeMacro(GrayLevelDifferenceCalculator,Object)

  void SetImage(SmartPointer<ImageType> image)
  {m_Image = image;}

  itkSetMacro(Distance,int)
  itkSetMacro(NumberOfBins,int)

  void Compute()
  {
    auto inputRegion = this->m_Image->GetLargestPossibleRegion();
    auto size = this->m_Image->GetLargestPossibleRegion().GetSize();
    auto index = this->m_Image->GetLargestPossibleRegion().GetIndex();

    typename InternalImageType::RegionType::SizeType
                                            internalSize = {size[0],size[1]};
    typename InternalImageType::RegionType::IndexType
                                         internalIndex = {index[0],index[1]};

    auto internalRegion = typename
                   InternalImageType::RegionType(internalIndex,internalSize);

    typedef ImageSliceConstIteratorWithIndex
                                         <ImageType>            IteratorType;
    typedef ImageLinearIteratorWithIndex
                              <InternalImageType>          WriteIteratorType;
    typedef ConstNeighborhoodIterator
                               <ImageType>          NeighborhoodIteratorType;

    typename NeighborhoodIteratorType::OffsetType
                                                 offset1 = {{m_Distance,0,0}};
    typename NeighborhoodIteratorType::OffsetType
                                       offset2 = {{m_Distance,-m_Distance,0}};
    typename NeighborhoodIteratorType::OffsetType
                                                 offset3 = {{0,m_Distance,0}};
    typename NeighborhoodIteratorType::OffsetType
                                      offset4 = {{-m_Distance,-m_Distance,0}};

    typename NeighborhoodIteratorType::RadiusType radius;
    radius.Fill(m_Distance);
    NeighborhoodIteratorType neighborhoodIterator(radius,m_Image,inputRegion);

    IteratorType iterator(m_Image,inputRegion);

    iterator.SetFirstDirection(0);
    iterator.SetSecondDirection(1);

    int sliceID = 0;
    iterator.GoToBegin();

    double fullP1[size[2]][m_NumberOfBins];
    double fullP2[size[2]][m_NumberOfBins];
    double fullP3[size[2]][m_NumberOfBins];
    double fullP4[size[2]][m_NumberOfBins];

    while(!iterator.IsAtEnd())
    {
      auto p1 = InternalImageType::New();
      auto p2 = InternalImageType::New();
      auto p3 = InternalImageType::New();
      auto p4 = InternalImageType::New();

      p1->SetRegions(internalRegion);
      p2->SetRegions(internalRegion);
      p3->SetRegions(internalRegion);
      p4->SetRegions(internalRegion);

      p1->Allocate();
      p2->Allocate();
      p3->Allocate();
      p4->Allocate();

      WriteIteratorType iteratorP1(p1,internalRegion);
      WriteIteratorType iteratorP2(p2,internalRegion);
      WriteIteratorType iteratorP3(p3,internalRegion);
      WriteIteratorType iteratorP4(p4,internalRegion);

      iteratorP1.SetDirection(0);
      iteratorP2.SetDirection(0);
      iteratorP3.SetDirection(0);
      iteratorP4.SetDirection(0);

      iteratorP1.GoToBegin();
      iteratorP2.GoToBegin();
      iteratorP3.GoToBegin();
      iteratorP4.GoToBegin();

      while (!iterator.IsAtEndOfSlice())
      {
        while (!iterator.IsAtEndOfLine())
        {
          auto pixel = iterator.Get();
          auto thisIndex = iterator.GetIndex();
          neighborhoodIterator.SetLocation(thisIndex);
          if (thisIndex[0]+m_Distance < size[0])
          {
            iteratorP1.Set(std::abs
                              (pixel - neighborhoodIterator.GetPixel(offset1)));
          } else
            {iteratorP1.Set(0);}

          if (thisIndex[1]-m_Distance >= 0 && thisIndex[0]+m_Distance < size[0])
          {
            iteratorP2.Set(std::abs
                            (pixel - neighborhoodIterator.GetPixel(offset2)));
          } else
            {iteratorP2.Set(0);}

          if (thisIndex[1]+m_Distance < size[1])
          {
            iteratorP3.Set(std::abs
                            (pixel - neighborhoodIterator.GetPixel(offset3)));
          } else
            {iteratorP3.Set(0);}

          if (thisIndex[1]-m_Distance >= 0 && thisIndex[0]-m_Distance >= 0)
          {
            iteratorP4.Set(std::abs
                           (pixel - neighborhoodIterator.GetPixel(offset4)));
          } else
            {iteratorP4.Set(0);}

          ++iterator;
          ++iteratorP1;
          ++iteratorP2;
          ++iteratorP3;
          ++iteratorP4;
        }
        iterator.NextLine();
        iteratorP1.NextLine();
        iteratorP2.NextLine();
        iteratorP3.NextLine();
        iteratorP4.NextLine();
      }

      int* currentP1 = CreateHistogram(p1);
      int* currentP2 = CreateHistogram(p2);
      int* currentP3 = CreateHistogram(p3);
      int* currentP4 = CreateHistogram(p4);

      for (int i = 0;i < m_NumberOfBins;i++)
      {
        fullP1[sliceID][i] = (double) currentP1[i];
        fullP2[sliceID][i] = (double) currentP2[i];
        fullP3[sliceID][i] = (double) currentP3[i];
        fullP4[sliceID][i] = (double) currentP4[i];
      }

      delete[] currentP1;
      delete[] currentP2;
      delete[] currentP3;
      delete[] currentP4;

      sliceID++;
      iterator.NextSlice();
    }

    double meanP1[m_NumberOfBins];
    double meanP2[m_NumberOfBins];
    double meanP3[m_NumberOfBins];
    double meanP4[m_NumberOfBins];

    for (int i = 0;i < m_NumberOfBins;i++)
    {
      double sumP1 = 0.0;
      double sumP2 = 0.0;
      double sumP3 = 0.0;
      double sumP4 = 0.0;
      for (int j = 0;j < size[2];j++)
      {
        sumP1 += fullP1[j][i];
        sumP2 += fullP2[j][i];
        sumP3 += fullP3[j][i];
        sumP4 += fullP4[j][i];
      }
      meanP1[i] = sumP1/(double)size[2];
      meanP2[i] = sumP2/(double)size[2];
      meanP3[i] = sumP3/(double)size[2];
      meanP4[i] = sumP4/(double)size[2];
    }

    std::cout << "P1:\n[";
    for(int b = 0;b < m_NumberOfBins;b++)
    {
      std::cout << meanP1[b];
      if (b < m_NumberOfBins-1)  {std::cout << ",";}
    }
    std::cout<< "]\n" << std::endl;

    std::cout << "\nP2:\n[";
    for(int b = 0;b < m_NumberOfBins;b++)
    {
      std::cout << meanP2[b];
      if (b < m_NumberOfBins-1)  {std::cout << ",";}
    }
    std::cout<< "]\n" << std::endl;

    std::cout << "\nP3:\n[";
    for(int b = 0;b < m_NumberOfBins;b++)
    {
      std::cout << meanP3[b];
      if (b < m_NumberOfBins-1)  {std::cout << ",";}
    }
    std::cout<< "]\n" << std::endl;

    std::cout << "\nP4:\n[";
    for(int b = 0;b < m_NumberOfBins;b++)
    {
      std::cout << meanP4[b];
      if (b < m_NumberOfBins-1)  {std::cout << ",";}
    }
    std::cout<< "]\n" << std::endl;
  }

protected:
  GrayLevelDifferenceCalculator() {}
  ~GrayLevelDifferenceCalculator() {}

private:
  GrayLevelDifferenceCalculator(const Self&);
  void operator=(const Self&);

  SmartPointer<ImageType> m_Image;
  int m_Distance;
  int m_NumberOfBins;

  int* CreateHistogram(InternalImageType* image)
  {
    typedef MinimumMaximumImageCalculator
                            <InternalImageType>          MinMaxCalculatorType;

    auto minMaxCalculator = MinMaxCalculatorType::New();
    minMaxCalculator->SetImage(image);
    minMaxCalculator->Compute();

    auto maximum = minMaxCalculator->GetMaximum();
    auto minimum = minMaxCalculator->GetMinimum();
    double binLength = (double) (maximum-minimum)/m_NumberOfBins;

    typedef ImageLinearConstIteratorWithIndex
                                    <InternalImageType>          IteratorType;
    IteratorType iterator(image,image->GetLargestPossibleRegion());

    int* bins = new int[m_NumberOfBins];
    for (int k = 0;k < m_NumberOfBins;k++)
      {bins[k] = 0;}
    iterator.SetDirection(0);
    while(!iterator.IsAtEnd())
    {
      while(!iterator.IsAtEndOfLine())
      {
        auto pixel = iterator.Get();
        int binIndex;
        if (pixel == maximum)  {binIndex = m_NumberOfBins-1;}

        else   {binIndex = (int) ((pixel-minimum)/binLength);}
        bins[binIndex] = bins[binIndex] + 1;
        ++iterator;
      }
      iterator.NextLine();
    }

    int integral = 0;
    for (int i = 0;i < m_NumberOfBins;i++)
    {
      integral += bins[i];
      bins[i] = integral;
    }

    return bins;
  }
};

} //End ITK namespace

#ifndef ITK_MANUALINSTANTIATION
#include "itkGrayLevelDifferenceCalculator.h"
#endif //End ITK_MANUAL_INSTANTIATION

#endif //End ITK_GRAYLEVELDIFFERENCECALCULATOR_H
