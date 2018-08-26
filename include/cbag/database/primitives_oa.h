/** \file primitives_oa.h
 *  \brief This file defines OpenAccess based shape and geometry primitives.
 *
 *  \author Eric Chang
 *  \date   2018/07/13
 */

#ifndef CBAG_DATABASE_PRIMITIVES_OA_H
#define CBAG_DATABASE_PRIMITIVES_OA_H

#include <oa/oaDesignDB.h>

namespace cbag {
typedef oa::oaCoord coord_t;
typedef oa::oaLayerNum lay_t;
typedef oa::oaPurposeNum purp_t;
typedef oa::oaDist dist_t;

// enums
using Orientation = oa::oaOrientEnum;
constexpr Orientation oR0 = Orientation::oacR0;
constexpr Orientation oR90 = Orientation::oacR90;
constexpr Orientation oR180 = Orientation::oacR180;
constexpr Orientation oR270 = Orientation::oacR270;
constexpr Orientation oMY = Orientation::oacMY;
constexpr Orientation oMYR90 = Orientation::oacMYR90;
constexpr Orientation oMX = Orientation::oacMX;
constexpr Orientation oMXR90 = Orientation::oacMXR90;

using PathStyle = oa::oaPathStyleEnum;
constexpr PathStyle psTruncate = PathStyle::oacTruncatePathStyle;
constexpr PathStyle psExtend = PathStyle::oacExtendPathStyle;
constexpr PathStyle psRound = PathStyle::oacRoundPathStyle;
constexpr PathStyle psVariable = PathStyle::oacVariablePathStyle;

using TextAlign = oa::oaTextAlignEnum;
constexpr TextAlign taUL = TextAlign::oacUpperLeftTextAlign;
constexpr TextAlign taCL = TextAlign::oacCenterLeftTextAlign;
constexpr TextAlign taLL = TextAlign::oacLowerLeftTextAlign;
constexpr TextAlign taUC = TextAlign::oacUpperCenterTextAlign;
constexpr TextAlign taCC = TextAlign::oacCenterCenterTextAlign;
constexpr TextAlign taLC = TextAlign::oacLowerCenterTextAlign;
constexpr TextAlign taUR = TextAlign::oacUpperRightTextAlign;
constexpr TextAlign taCR = TextAlign::oacCenterRightTextAlign;
constexpr TextAlign taLR = TextAlign::oacLowerRightTextAlign;

using Font = oa::oaFontEnum;
constexpr Font fEuro = Font::oacEuroStyleFont;
constexpr Font fGoth = Font::oacGothicFont;
constexpr Font fMath = Font::oacMathFont;
constexpr Font fRoman = Font::oacRomanFont;
constexpr Font fScript = Font::oacScriptFont;
constexpr Font fStick = Font::oacStickFont;
constexpr Font fFixed = Font::oacFixedFont;
constexpr Font fSwedish = Font::oacSwedishFont;
constexpr Font fMilSpec = Font::oacMilSpecFont;

using TermAttrType = oa::oaTermAttrTypeEnum;
constexpr TermAttrType tatName = TermAttrType::oacNameTermAttrType;
constexpr TermAttrType tatHasPin = TermAttrType::oacHasPinsTermAttrType;
constexpr TermAttrType tatNumBits = TermAttrType::oacNumBitsTermAttrType;

using TextDispFormat = oa::oaTextDisplayFormatEnum;
constexpr TextDispFormat tdfName = TextDispFormat::oacNameTextDisplayFormat;
constexpr TextDispFormat tdfValue = TextDispFormat::oacValueTextDisplayFormat;
constexpr TextDispFormat tdfNameValue = TextDispFormat::oacNameValueTextDisplayFormat;

using SigType = oa::oaSigTypeEnum;
constexpr SigType stSignal = SigType::oacSignalSigType;
constexpr SigType stPower = SigType::oacPowerSigType;
constexpr SigType stGround = SigType::oacGroundSigType;
constexpr SigType stClock = SigType::oacClockSigType;
constexpr SigType stTieoff = SigType::oacTieoffSigType;
constexpr SigType stTieHi = SigType::oacTieHiSigType;
constexpr SigType stTieLo = SigType::oacTieLoSigType;
constexpr SigType stAnalog = SigType::oacAnalogSigType;
constexpr SigType stScan = SigType::oacScanSigType;
constexpr SigType stReset = SigType::oacResetSigType;

using TermType = oa::oaTermTypeEnum;
constexpr TermType trmInput = TermType::oacInputTermType;
constexpr TermType trmOutput = TermType::oacOutputTermType;
constexpr TermType trmInout = TermType::oacInputOutputTermType;

using BlockageType = oa::oaBlockageTypeEnum;
constexpr BlockageType blkRoute = BlockageType::oacRoutingBlockageType;
constexpr BlockageType blkPlacement = BlockageType::oacPlacementBlockageType;

using Point = oa::oaPoint;
using Vector = oa::oaVector;
using Transform = oa::oaTransform;
using BBox = oa::oaBox;
using PointArray = oa::oaPointArray;
using ViaParam = oa::oaViaParam;
} // namespace cbag

#endif // CBAG_DATABASE_PRIMITIVES_OA_H
