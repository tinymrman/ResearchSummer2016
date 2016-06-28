#include "itkHaarWaveletImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkCropImageFilter.h"

int main(int argc,char** argv)
{
  if (argc < 3)
  {
    std::cerr << "The CMD arguments were not formatted correctly!"
                << std::endl;
    return EXIT_FAILURE;
  }

  typedef float                                     PixelType;
  typedef itk::Image<PixelType,3>                   InputImageType;
  typedef itk::Image<PixelType,3>                   OutputImageType;
  typedef itk::ImageFileReader<InputImageType>      ReaderType;
  typedef itk::ImageFileWriter<OutputImageType>     WriterType;

  typedef itk::HaarWaveletImageFilter<InputImageType,OutputImageType>
                                                    FilterType;

  typedef itk::CropImageFilter<InputImageType,OutputImageType>
                                                    CropFilterType;

  auto reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  auto cropFilter = CropFilterType::New();
  cropFilter->SetInput(reader->GetOutput());

  typename InputImageType::SizeType cropSize;
  cropSize.Fill(0);

  auto size = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  std::cout << "Image Size:" << size << std::endl;
  for (int i = 0;i < InputImageType::ImageDimension;i++)
    {if (size[i] % 2 != 0)    {cropSize[i] = 1;}}

  std::cout << "Cropped: " << cropSize;
  cropFilter->SetLowerBoundaryCropSize(cropSize);

  auto filter = FilterType::New();
  filter->SetInput(cropFilter->GetOutput());

  auto writer = WriterType::New();
  writer->SetFileName(argv[2]);

  writer->SetInput(filter->GetOutput());
  writer->Update();

  std::cout << "\nThe Image was succesfully written to "
                 << argv[2] << std::endl;
  return EXIT_SUCCESS;
}
