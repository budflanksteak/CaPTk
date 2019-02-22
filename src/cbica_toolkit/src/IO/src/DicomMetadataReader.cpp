#include "DicomMetadataReader.h"

DicomMetadataReader::DicomMetadataReader()
{
  m_reader = ReaderType::New();
  m_dicomIO = ImageIOType::New();
  m_nameGenerator = NamesGeneratorType::New();
  m_reader->SetImageIO(m_dicomIO);
  m_tagvalueMap = MapContainerType::New();
}

DicomMetadataReader::~DicomMetadataReader()
{
}

void DicomMetadataReader::SetDirectoryPath(std::string path)
{
  m_nameGenerator->SetInputDirectory(path);
}

void DicomMetadataReader::ReadMetaData()
{
  m_tagvalueMap->Initialize(); //clear the map
  m_fileNames = m_nameGenerator->GetInputFileNames();
  m_reader->SetFileNames(m_fileNames);

  try
  {
    m_reader->Update();
  }
  catch (itk::ExceptionObject &ex)
  {
    //TBD: handle this
    std::cout << ex << std::endl;
  }

  m_dictionary = m_dicomIO->GetMetaDataDictionary();

  DictionaryType::ConstIterator itr = m_dictionary.Begin();
  DictionaryType::ConstIterator end = m_dictionary.End();

  while (itr != end)
  {
    itk::MetaDataObjectBase::Pointer  entry = itr->second;

    MetaDataStringType::Pointer entryvalue = dynamic_cast<MetaDataStringType *>(entry.GetPointer());

    std::string label;
    std::pair<std::string, std::string> labelValuePair;
    if (entryvalue)
    {
      std::string tagkey = itr->first;
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      bool found = itk::GDCMImageIO::GetLabelFromTag(tagkey, label);
      if (found)
      {
        labelValuePair.first = label;
        labelValuePair.second = tagvalue;
        m_tagvalueMap->InsertElement(tagkey, labelValuePair);
      }
    }
    ++itr;
  }
}

void DicomMetadataReader::PrintMetaData()
{
  MapContainerType::Iterator itr;
  std::pair<std::string, std::string> labelValuePair;
  for (itr = m_tagvalueMap->Begin(); itr != m_tagvalueMap->End(); ++itr)
  {
    labelValuePair = itr->Value();
    std::cout << itr->Index() << " " << labelValuePair.first << " " << labelValuePair.second << std::endl;
  }
}

std::map<std::string, std::pair<std::string, std::string>> DicomMetadataReader::GetMetaDataMap()
{
  return m_tagvalueMap->CastToSTLContainer();
}

bool DicomMetadataReader::GetTagValue(std::string tag, std::string &label, std::string &value)
{
  bool status = false;
  DictionaryType::ConstIterator tagItr = m_dictionary.Find(tag);
  if (tagItr == m_dictionary.End())
    status = false;
  else
  {
    MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>(tagItr->second.GetPointer());
    if (entryvalue)
    {
      value = entryvalue->GetMetaDataObjectValue();
      status = itk::GDCMImageIO::GetLabelFromTag(tag, label);
    }
    else
    {
      status = false; //entry was not string
    }
  }

  return status;
}
