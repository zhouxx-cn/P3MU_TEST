#include "global_macro.h"
#include "idvr_errno.h"
#include "PAR/CConfig.h"
#include "CPtzEletronic.h"
#include "tagPtzCode.h"
#include "PTZTrace.h"

const char *CPtzEletronic::m_aszEletronicPropName[] = {
	"DigitZoom",
	"LowLum",
	"BackLight",
	"AutoShutter",
	"WhiteBalance",
	"RedGain",
	"BlueGain",
	"Acutance",
	"ExpMode",
	"ShutterSpeed",
	"IrisSpeed",
	"WideDyn",
	"3DFilter",
	"Mirror",
	"Reverse",
	"HDVideoMode",
	"limitSpeed",
	"iLowPowerConsumption",
	"iScreenWiper",
	"iNightVision",
	"iDemist",
	"",
	"",
};

int CPtzEletronic::m_nSonyDefValue[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0xFF00FF
};

CPtzEletronic::CPtzEletronic(int nIndex):m_nChannel(nIndex), m_nDevType(0)
{
}

CPtzEletronic::~CPtzEletronic()
{

}

int CPtzEletronic::Init(int nDevType)
{
	int nPropIndex = 0;

	int *pValue = NULL;

	switch (nDevType >> 16)
	{
		case 0:
			m_nDevType = 0;
			return 0;
			break;
		case eSony:
		default:
			pValue = m_nSonyDefValue;
			break;
	}

	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	for (; nPropIndex < (int) ePropEnd; nPropIndex++)
	{
		m_nEletronicProp[nPropIndex] = pConfig->GetValue(szSectionName, m_aszEletronicPropName[nPropIndex], pValue[nPropIndex]);
		PTZ_DBG("Ch=%d, Prop=%d, Value=%d\n", m_nChannel, nPropIndex, m_nEletronicProp[nPropIndex]);
	}
	m_nDevType = nDevType;

	return 0;
}

int CPtzEletronic::GetType()
{
	return m_nDevType;
}

int CPtzEletronic::GetEletronicProp(int nIndex)
{
	PTZ_DBG("Get Eletronic Param, Ch=%d, Index=%d, Value=%d\n", m_nChannel, nIndex, m_nEletronicProp[nIndex]);

	// NOTE: Requires a correct return value to compile.
	return m_nEletronicProp[nIndex];
}

int CPtzEletronic::SetEletronicProp(int nIndex, int nValue)
{
	PTZ_DBG("Set Eletronic Param, Ch=%d, Index=%d, Value=%d\n", m_nChannel, nIndex, nValue);
	m_nEletronicProp[nIndex] = nValue;
	char szSectionName[20];

	snprintf(szSectionName,sizeof(szSectionName), "PTZ_Set_%d", m_nChannel);
	CConfig *pConfig = CConfig::GetInstance();

	pConfig->SetValue(szSectionName, m_aszEletronicPropName[nIndex], nValue);

	return (int) 0;
}

int CPtzEletronic::ResetAllEletronicProp()
{
	return (int) 0;
}
