#include "itkLocalBinaryPatternImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"

int main (int argc,char** argv)
{
  if (argc < 3)
  {
    std::cerr << "The CMD arguments were not formatted correctly!"
              << std::endl;
    return EXIT_FAILURE;
  }

  typedef int                                               PixelType;
  typedef itk::Image<PixelType,3>                           ImageType;
  typedef itk::ImageFileReader<ImageType>                   ReaderType;
  typedef itk::ImageFileWriter<ImageType>                   WriterType;

  typedef itk::LocalBinaryPatternImageFilter<ImageType>     FilterType;

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

  auto filter = FilterType::New();
  filter->SetInput(reader->GetOutput());
  filter->Update();

  auto writer = WriterType::New();
  writer->SetFileName(argv[2]);

  typedef itk::RescaleIntensityImageFilter<ImageType,ImageType> RescaleFilterType;

  auto rescaler = RescaleFilterType::New();

  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);

  rescaler->SetInput(filter->GetOutput());
  writer->SetInput(rescaler->GetOutput());
  writer->Update();

  return EXIT_SUCCESS;
}
