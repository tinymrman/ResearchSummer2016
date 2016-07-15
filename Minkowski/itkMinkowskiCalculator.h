#ifndef ITK_MINKOWSKICALCULATOR_H
#define ITK_MINKOWSKICALCULATOR_H

#include "itkImageToImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageSliceIteratorWithIndex.h"

namespace itk
{

template <class TImage>
class MinkowskiCalculator : public ImageToImageFilter
<TImage,TImage>
{
public:
  typedef MinkowskiCalculator                                            Self;
  typedef Object                                                   Superclass;
  typedef SmartPointer<Self>                                          Pointer;

  itkNewMacro(Self)
  itkTypeMacro(MinkowskiCalculator,Object)

  typedef TImage                                                    ImageType;

protected:
  MinkowskiCalculator() {}
  ~MinkowskiCalculator() {}

  virtual void GenerateData()
  {
    typedef itk::ImageSliceConstIteratorWithIndex<ImageType>    IteratorType;
    typedef itk::ImageSliceIteratorWithIndex<ImageType>   OutputIteratorType;
    typedef itk::ConstNeighborhoodIterator
                                   <ImageType>      NeighborhoodIteratorType;

    auto input = this->GetInput();
    auto size = input->GetLargestPossibleRegion().GetSize();
    int pixelTotal = size[0]*size[1];
    int areas[size[2]];
    int perimeters[size[2]];
    int genii[size[2]];
    double nAreas[size[2]];
    double nPerimeters[size[2]];

    auto output = this->GetOutput();
    output->SetRegions(input->GetLargestPossibleRegion());
    output->Allocate();

    typename NeighborhoodIteratorType::RadiusType    radius;
    radius.Fill(1);

    IteratorType       iterator(input,input->GetLargestPossibleRegion());
    OutputIteratorType outputIterator
                              (output,input->GetLargestPossibleRegion());
    NeighborhoodIteratorType neighborhoodIterator
                        (radius,input,input->GetLargestPossibleRegion());

    iterator.SetFirstDirection(0);
    iterator.SetSecondDirection(1);
    outputIterator.SetFirstDirection(0);
    outputIterator.SetSecondDirection(1);

    typename NeighborhoodIteratorType::OffsetType offset1 = {{-1,-1,0}};
    typename NeighborhoodIteratorType::OffsetType offset2 = {{0,-1,0}};
    typename NeighborhoodIteratorType::OffsetType offset3 = {{1,-1,0}};
    typename NeighborhoodIteratorType::OffsetType offset4 = {{-1,0,0}};

    int sliceID = 0;

    outputIterator.GoToBegin();
    iterator.GoToBegin();
    while (!outputIterator.IsAtEnd())
    {
      int visiblePixels = 0;
      int visibleEdges = 0;
      int visibleVertices = 0;

      while(!outputIterator.IsAtEndOfSlice())
      {
        while (!outputIterator.IsAtEndOfLine())
        {
          auto thisPixel = iterator.Get();
          outputIterator.Set(thisPixel);
          if (thisPixel == 1.0)
          {
            neighborhoodIterator.SetLocation(iterator.GetIndex());
            ++visiblePixels;
            visibleEdges += 4;
            visibleVertices += 4;
            if (neighborhoodIterator.GetPixel(offset4) == 1.0)
            {
              --visibleEdges;
              --visibleVertices;
            }
            if (neighborhoodIterator.GetPixel(offset2) == 1.0) {--visibleEdges;}
            if (neighborhoodIterator.GetPixel(offset2) == 1.0 ||
                neighborhoodIterator.GetPixel(offset3) == 1.0)
                  {--visibleVertices;}
            if (neighborhoodIterator.GetPixel(offset1) == 1.0 ||
                neighborhoodIterator.GetPixel(offset2) == 1.0 ||
                neighborhoodIterator.GetPixel(offset4) == 1.0)
                  {--visibleVertices;}
          }
          ++iterator;
          ++outputIterator;
        }
        iterator.NextLine();
        outputIterator.NextLine();
      }
      double normalizedArea = (double) visiblePixels/pixelTotal;
      double perimeter = (2*visibleEdges)-(4*visiblePixels);
      double normalizedPerimeter = (double) perimeter/(pow(pixelTotal,0.5));
      int genus = visiblePixels+visibleVertices-visibleEdges;

      areas[sliceID] = visiblePixels;
      perimeters[sliceID] = perimeter;
      genii[sliceID] = genus;
      nAreas[sliceID] = normalizedArea;
      nPerimeters[sliceID] = normalizedPerimeter;

      iterator.NextSlice();
      outputIterator.NextSlice();
      sliceID++;
    }

    int areaSum = 0;
    int perSum = 0;
    int genSum = 0;
    double nAreaSum = 0;
    double nPerSum = 0;
    for (int j = 0;j < size[2];j++)
    {
      areaSum += areas[j];
      perSum += perimeters[j];
      genSum += genii[j];
      nAreaSum += nAreas[j];
      nPerSum += nPerimeters[j];
    }
    double areaMean = (double) areaSum/size[2];
    double perimeterMean = (double) perSum/size[2];
    double genusMean = (double) genSum/size[2];
    double nAreaMean = (double) nAreaSum/size[2];
    double nPerimeterMean = (double) nPerSum/size[2];

    double areaSD = 0;
    double perSD = 0;
    double genSD = 0;
    double nAreaSD = 0;
    double nPerSD = 0;

    for (int k = 0;k < size[2];k++)
    {
      areaSD += (double) pow(areas[k]-areaMean,2)/size[2];
      perSD += (double) pow(perimeters[k]-perimeterMean,2)/size[2];
      genSD += (double) pow(genii[k]-genusMean,2)/size[2];
      nAreaSD += (double) pow(nAreas[k]-nAreaMean,2)/size[2];
      nPerSD += (double) pow(nPerimeters[k]-nPerimeterMean,2)/size[2];
    }

    areaSD = pow(areaSD,0.5);
    perSD = pow(perSD,0.5);
    genSD = pow(genSD,0.5);
    nAreaSD = pow(nAreaSD,0.5);
    nPerSD = pow(nPerSD,0.5);

    std::cout << "Area Mean: " << areaMean << ", Perimeter Mean: " <<
            perimeterMean << ", Genus Mean: " << genusMean <<
            ", Normalized Area Mean: " << nAreaMean <<
            ", Normalized Perimeter Mean: " << nPerimeterMean << "\n" <<
            "Area SD: " << areaSD << ", Perimeter SD: " <<
            perSD << ", Genus SD: " << genSD <<
            ", Normalized Area SD: " << nAreaSD <<
            ", Normalized Perimeter SD: " << nPerSD << "\n\n";
  }

private:
  MinkowskiCalculator(const Self&);
  void operator=(const Self&);
};

} //End ITK namespace

#ifndef ITK_MANUALINSTANTIATION
#include "itkMinkowski.h"
#endif //End ITK_MANUAL_INSTANTIATION

#endif
