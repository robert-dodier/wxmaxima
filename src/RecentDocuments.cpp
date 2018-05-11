#include "RecentDocuments.h"
#include <wx/filename.h>

RecentDocuments::RecentDocuments(wxString documentType) :   m_documentType(documentType)
{
  Load();
}

void RecentDocuments::Load()
{
  wxConfigBase *config = wxConfig::Get();
  wxString fileName;

  for(int i=0; i<30; i++)
    {
      wxString name = wxString::Format(wxT("RecentDocuments/%s_%d"), m_documentType, i);
      wxString filename;
      if (config->Read(name, &filename))
	{
	  if(filename != wxEmptyString)
	    m_listOfFiles.push_back(filename);
	}
    }  
  m_listOfFiles.unique();
  
  Save();
}

wxString RecentDocuments::Get(int num)
{
  std::list<wxString> listOfFiles = m_listOfFiles;
  for(int i = 0; i<num; i++)
    {
      if(listOfFiles.empty())
	return wxEmptyString;
      else
	listOfFiles.pop_front();      
    }
  if(listOfFiles.empty())
    return wxEmptyString;
  else
    return listOfFiles.front();
}

void RecentDocuments::Save()
{
  wxConfigBase *config = wxConfig::Get();
  int i = 0;
  for(std::list<wxString>::iterator it = m_listOfFiles.begin(); it != m_listOfFiles.end();++it)
    {
      wxString name = wxString::Format(wxT("RecentDocuments/%s_%d"), m_documentType, i);
      i++;
      config->Write(name, *it);
    }
  for(;i<30;i++)
    {
      wxString name = wxString::Format(wxT("RecentDocuments/%s_%d"), m_documentType, i);
      config->DeleteEntry(name);
    }
}

void RecentDocuments::AddDocument(wxString filename)
{
  wxFileName fle(filename);
  fle.MakeAbsolute();
  if((fle.GetFullPath() != wxEmptyString) && wxFileExists(fle.GetFullPath()))
    filename = fle.GetFullPath();

  for(std::list<wxString>::iterator it = m_listOfFiles.begin(); it != m_listOfFiles.end();++it)
    if (*it == filename)
      return;
  
  m_listOfFiles.push_front(filename);
  m_listOfFiles.unique();
  long recentItems;
  wxConfig::Get()->Read(wxT("recentItems"), &recentItems);
  if (recentItems < 5) recentItems = 5;
  if (recentItems > 30) recentItems = 30;
  while(m_listOfFiles.size() > recentItems)
    m_listOfFiles.pop_back();
  Save();
}
