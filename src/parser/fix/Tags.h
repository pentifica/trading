#pragma once
/// @copyright {2023, Russell J. Fleming. All rights reserved.}
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
#include    <cstdint>
#include    <cstddef>

namespace pentifica::trd::fix {
/// @brief Identifies the versions of the FIX protocol currently supported
enum class Version {
    _4_2,
};

    using Byte = unsigned char;
    /// @brief Identifies the tags for fields in a message
    enum class Tag:uint32_t {
        Account = 1,
        AggressorIndicator = 1057,
        ApplicationSystemName = 1603,
        ApplicationSystemVendor = 1605,
        ApplicationSystemVersion = 1604,
        AvgPx = 6,
        BeginSeqNo = 7,
        BeginString = 8,
        BodyLength = 9,
        BusinessRejectReason = 380,
        CheckSum = 10,
        ClOrdID = 11,
        ContraBroker = 375,
        ContraTrader = 337,
        CorrelationClOrdID = 9717,
        CrossID = 548,
        CrossType = 549,
        CtiCode = 9702,
        CumQty = 14,
        CustomerOrFirm = 204,
        CxlRejReason = 102,
        CxlRejResponseTo = 434,
        DisplayFactor = 9787,
        EncryptMethod = 98,
        EndSeqNo = 16,
        EventDate = 866,
        EventPx = 867,
        EventText = 868,
        EventTime = 1145,
        EventType = 865,
        ExecID = 17,
        ExecInst = 18,
        ExecTransType = 20,
        ExecType = 150,
        ExpirationTimeValue = 1189,
        ExpireDate = 432,
        GapFillFlag = 123,
        HandlInst = 21,
        HeartBtInt = 108,
        LastMsgSeqNumProcessed = 369,
        LastPx = 31,
        LastShares = 32,
        LeavesQty = 151,
        LegCFICode = 608,
        LegCurrency = 556,
        LegMaturityMonthYear = 610,
        LegOptionDelta = 1017,
        LegPrice = 566,
        LegRatioQty = 623,
        LegSecurityID = 602,
        LegSecurityIDSource = 603,
        LegSecuritySubType = 764,
        LegSide = 624,
        LegStrikeCurrency = 942,
        LegStrikePrice = 612,
        LegSymbol = 600,
        ManualOrderIndicator = 1028,
        MarketDepth = 264,
        MaxShow = 210,
        MDBookType = 1021,
        MDFeedType = 1022,
        MinPriceIncrement = 969,
        MinQty = 110,
        MsgSeqNum = 34,
        MsgType = 35,
        MultiLegReportingType = 442,
        NewSeqNo = 36,
        NextExpectedMsgSeqNum = 789,
        NoEvents = 864,
        NoLegs = 555,
        NoMdFeedTypes = 1141,
        NoRelatedSym = 146,
        NoSecurityAltID = 454,
        NoUnderlyings = 711,
        OFMOverride = 9768,
        OpenClose = 77,
        OptionDelta = 811,
        OrderID = 37,
        OrderQty = 38,
        OrdRejReason = 103,
        OrdStatus = 39,
        OrdType = 40,
        OrigClOrdID = 41,
        OrigSendingTime = 122,
        PossDupFlag = 43,
        PossResend = 97,
        Price = 44,
        RawData = 96,
        RawDataLength = 95,
        RefMsgType = 372,
        RefSeqNum = 45,
        ResetSeqNumFlag = 141,
        RiskFreeRate = 1190,
        SecondaryExecID = 527,
        SecurityDesc = 107,
        SecurityGroup = 1151,
        SecurityID = 48,
        SecurityResponseType = 323,
        SecurityType = 167,
        SenderCompID = 49,
        SenderLocationID = 142,
        SenderSubID = 50,
        SendingTime = 52,
        Side = 54,
        StopPx = 99,
        Symbol = 55,
        TargeLocationID = 143,
        TargetCompID = 56,
        TargetSubID = 57,
        TestReqID = 112,
        Text = 58,
        TimeInForce = 59,
        TotalNumSecurities = 393,
        TradeDate = 75,
        TransactTime = 60,
        UnderlyingSecurityID = 309,
        UnderlyingPx = 810,
        UnderlyingSecurityIDSource = 305,
        UnderlyingSymbol = 311,
        Volatility = 1188,
    };
    /// @brief Provides the width (in chars) for some fields in a fix message
    enum class TagWidth:uint32_t {
        Account = 12,
        ApplicationSystemName = 30,
        ApplicationSystemVendor = 10,
        ApplicationSystemVersion = 10,
        BodyLength = 6,
        ClOrdID = 20,
        CorrelationClOrdID = 5,
        CtiCode = 1,
        CustomerOrFirm = 1,
        ExecID=40,
        ExecID_WIDTH = 40,
        LastMsgSeqNumProcessed = 9,
        ManualOrderIndicator = 1,
        MsgSeqNum = 9,
        RawData = 20,
        SenderCompID = 7,
        SenderLocationID = 32,
        SenderSubID = 20,
        SendingTime = 21,
        TargetCompID = 7,
        TargetSubID = 20,
        TestReqID = 20,
        Text = 150,
    };
    /// @brief Identifies the fix message types
    enum class MsgType:Byte {
        LOGON = 'A',
        HEARTBEAT = '0',
        TEST_REQUEST = '1',
        RESEND_REQUEST = '2',
        SESSION_LEVEL_REJECT = '3',
        BUSINESS_LEVEL_REJECT = 'j',
        SEQUENCE_RESET = '4',
        LOGOUT = '5',
        ORDER_CANCEL_REJECT = '9',
        EXECUTION_REPORT = '8',
        NEW_ORDER = 'D',
        CANCEL_ORDER = 'F',
        REVISE_ORDER = 'G',
    };
    /// @brief Tag separator
    constexpr Byte SOH{1};
}
