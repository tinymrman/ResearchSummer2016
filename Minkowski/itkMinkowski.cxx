#include "itkMinkowski.h"
#include "itkMinkowskiCalculator.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSliceBySliceImageFilter.h"
#include "itkAbsImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

int main(int argc,char** argv)
{
  if (argc < 2)
  {
    std::cerr << "The CMD arguments were not formatted correctly!"
              << std::endl;
    return EXIT_FAILURE;
  }

  typedef float                                                     PixelType;
  typedef float                                               BinaryPixelType;
  typedef itk::Image<PixelType,3>                                   ImageType;
  typedef itk::Image<PixelType,2>                               TwoDImageType;
  typedef itk::Image<BinaryPixelType,3>                 ThreeDBinaryImageType;
  typedef itk::Image<BinaryPixelType,2>                       BinaryImageType;
  typedef itk::ImageFileReader<ImageType>                          ReaderType;
  typedef itk::ImageFileWriter<ThreeDBinaryImageType>              WriterType;

  typedef itk::Minkowski<TwoDImageType,TwoDImageType>              FilterType;
  typedef itk::SliceBySliceImageFilter
                          <ImageType,ThreeDBinaryImageType>   SliceFilterType;
  typedef itk::BinaryThresholdImageFilter
                          <TwoDImageType,BinaryImageType> ThresholdFilterType;
  typedef itk::MinkowskiCalculator<ThreeDBinaryImageType>      CalculatorType;


  auto reader = ReaderType::New();
  reader->SetFileName(argv[1]);

  try
  {
    reader->Update();

  } catch(itk::ExceptionObject& eo)
  {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << eo << std::endl;
    return EXIT_FAILURE;
  }

  auto sliceFilter = SliceFilterType::New();
  auto minkowskiFilter = FilterType::New();
  auto thresholdFilter = ThresholdFilterType::New();
  auto calculatorFilter = CalculatorType::New();
  //auto writer = WriterType::New();
  thresholdFilter->SetInput(minkowskiFilter->GetOutput());
  float upperThreshold = 1.0;
  float insideValue = 1;
  float outsideValue = 0;

  thresholdFilter->SetUpperThreshold(upperThreshold);
  thresholdFilter->SetInsideValue(insideValue);
  thresholdFilter->SetOutsideValue(outsideValue);

  sliceFilter->SetInput(reader->GetOutput());
  sliceFilter->SetInputFilter(minkowskiFilter);

  double lowerThreshold = 0.1;

  for (int iteration = 1;iteration < 11;iteration++)
  {
    std::cout << "Threshold Level: " << lowerThreshold << '\n';
    thresholdFilter->SetLowerThreshold(lowerThreshold);
    thresholdFilter->Modified();

    sliceFilter->SetOutputFilter(thresholdFilter);
    sliceFilter->Modified();

    calculatorFilter->SetInput(sliceFilter->GetOutput());
    calculatorFilter->Modified();
    calculatorFilter->Update();

    //std::string niftiName = std::string(argv[2]) +
    //        std::to_string(iteration) + std::string(".nii.gz");
    //writer->SetFileName(niftiName);

    //writer->SetInput(calculatorFilter->GetOutput());
    //writer->Modified();
    //writer->Update();
    lowerThreshold += 0.1;
  }

  return EXIT_SUCCESS;
}
