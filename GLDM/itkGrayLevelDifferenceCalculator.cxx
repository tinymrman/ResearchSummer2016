#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkGrayLevelDifferenceCalculator.h"

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "The CMD arguments were not formatted correctly!"
              << std::endl;
    return EXIT_FAILURE;
  }

  typedef float                                                     PixelType;
  typedef itk::Image<PixelType,3>                                   ImageType;
  typedef itk::Image<PixelType,2>                           InternalImageType;
  typedef itk::ImageFileReader<ImageType>                          ReaderType;
  typedef itk::GrayLevelDifferenceCalculator
                          <ImageType,InternalImageType>        CalculatorType;

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

  auto calculator = CalculatorType::New();
  calculator->SetImage(reader->GetOutput());
  calculator->SetDistance(atoi(argv[2]));
  calculator->SetNumberOfBins(atoi(argv[3]));
  calculator->Modified();

  calculator->Compute();

  return EXIT_SUCCESS;
}
