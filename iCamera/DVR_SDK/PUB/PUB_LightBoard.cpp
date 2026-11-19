
#include "PUB_LightBoard.h"

int PUB_LightBoard_GetCapbility (LightBoardType eLBType, PUB_LightBoard_Capbility *psCap)
{
    switch (eLBType)
    {
        case LightBoardType_DoubleHardLightSensitive:
        case LightBoardType_DoubleSoftLightSensitive:
        {
            psCap->bSupportColorNight = 1;
            psCap->bSupportIntelligentNight = 1;
            psCap->bSupportVariableInfrared = 0;
            psCap->bSupportVariableWhite = 0;
            psCap->bSupportFaceNoExposure = 1;
            psCap->bSupportLicensePlate = 1;
            psCap->VariableInfraredValueFixed = 1;
            psCap->VariableWhiteFixed = 1;
            break;
        }
        case LightBoardType_InfiniteWhiteLight:
        {
            psCap->bSupportColorNight = 0;
            psCap->bSupportIntelligentNight = 0;
            psCap->bSupportVariableInfrared = 0;
            psCap->bSupportVariableWhite = 1;
            psCap->bSupportFaceNoExposure = 1;
            psCap->bSupportLicensePlate = 1;
            psCap->VariableInfraredValueFixed = 1;
            psCap->VariableWhiteFixed = 1;
            break;
        }
        default:
        { /* 在摇头机里只用这么写 */
            psCap->bSupportColorNight = 0;
            psCap->bSupportIntelligentNight = 0;
            psCap->bSupportVariableInfrared = 0;
            psCap->bSupportVariableWhite = 0;
            psCap->bSupportFaceNoExposure = 1;
            psCap->bSupportLicensePlate = 1;
            psCap->VariableInfraredValueFixed = 0;
            psCap->VariableWhiteFixed = 0;
            break;
        }
    }

    return 0;
}
