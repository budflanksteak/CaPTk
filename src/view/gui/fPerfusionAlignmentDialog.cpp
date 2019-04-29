#include "fPerfusionAlignmentDialog.h"
//#include "fProgressDialog.h"
//#include "CAPTk.h"
#include "CaPTkGUIUtils.h"

#include "cbicaITKUtilities.h"

fPerfusionAligner::fPerfusionAligner()
{
  setupUi(this);
  this->setWindowModality(Qt::NonModal);
  //this->setModal(true); // this is a pre-processing routine and therefore should be modal
  this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(CancelButtonPressed()));
  connect(confirmButton, SIGNAL(clicked()), this, SLOT(ConfirmButtonPressed()));
  connect(outputImageButton, SIGNAL(clicked()), this, SLOT(SelectOutputImage()));
  connect(inputImageButton, SIGNAL(clicked()), this, SLOT(SelectInputImage()));

  m_rcbv->setChecked(true);
  m_psr->setChecked(true);
  m_ph->setChecked(true);
}
fPerfusionAligner::~fPerfusionAligner()
{
}
void fPerfusionAligner::CancelButtonPressed()
{
  this->close();
}
void fPerfusionAligner::ConfirmButtonPressed()
{
  if ((inputImageName->text().isEmpty()))
  {
    ShowErrorMessage("Please specify the DSC-MRI Image.");
    return;
  }
  if ((inputEchoName->text().isEmpty()))
  {
	  ShowErrorMessage("Please specify the Echo Time.");
	  return;
  }
  if (outputImageName->text().isEmpty())
  {
    ShowErrorMessage("Please specify the output folder.");
    return;
  }
  if (m_rcbv->isChecked() == false && m_psr->isChecked() == false && m_ph->isChecked() == false)
  {
    ShowErrorMessage("Please select at least one of the given three options: ap-rCBV, PH, PSR.");
    return;
  }
  emit RunPerfusionMeasuresCalculation(inputEchoName->text().toDouble(), m_rcbv->isChecked(), m_psr->isChecked(), m_ph->isChecked(), mInputPathName.toStdString(), mOutputPathName.toStdString());

  this->close();
}

void fPerfusionAligner::SelectOutputImage()
{
  QString directory = getExistingDirectory(this, mOutputPathName);
  if (directory.isNull() || directory.isEmpty())
    return;
  else
    outputImageName->setText(directory);

  mOutputPathName = directory;
}

void fPerfusionAligner::SelectInputImage()
{
	auto inputImage = getExistingFile(this, mInputPathName);
	if (inputImage.isNull() || inputImage.isEmpty())
		return;
	else
		inputImageName->setText(inputImage);

	mInputPathName = inputImage;
}
