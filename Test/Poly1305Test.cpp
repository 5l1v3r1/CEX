#include "Poly1305Test.h"
#include "../CEX/Poly1305.h"

namespace Test
{
	const std::string Poly1305Test::DESCRIPTION = "Poly1305 MAC Generator Tests.";
	const std::string Poly1305Test::FAILURE = "FAILURE! ";
	const std::string Poly1305Test::SUCCESS = "SUCCESS! Poly1305 tests have executed succesfully.";

	Poly1305Test::Poly1305Test()
		:
		m_progressEvent()
	{
		Initialize();
	}

	Poly1305Test::~Poly1305Test()
	{
	}

	const std::string Poly1305Test::Description()
	{
		return DESCRIPTION;
	}

	TestEventHandler &Poly1305Test::Progress()
	{
		return m_progressEvent;
	}

	std::string Poly1305Test::Run()
	{
		try
		{
			const size_t SEQLEN = 16;
			for (size_t i = 0; i < SEQLEN; ++i)
			{
				Poly1305Compare(m_key[i], m_plainText[i], m_expectedCode[i]);
			}

			OnProgress(std::string("Poly1305Test: Passed Poly1305 sequential known answer vector tests.."));

			for (size_t i = SEQLEN; i < m_key.size(); ++i)
			{
				Poly1305AESCompare(m_key[i], m_nonce[i - SEQLEN], m_plainText[i], m_expectedCode[i]);
			}

			OnProgress(std::string("Poly1305Test: Passed Poly1305-AES known answer vector tests.."));

			return SUCCESS;
		}
		catch (TestException const &ex)
		{
			throw TestException(FAILURE + std::string(" : ") + ex.Message());
		}
		catch (...)
		{
			throw TestException(std::string(FAILURE + std::string(" : Unknown Error")));
		}
	}

	void Poly1305Test::Poly1305Compare(std::vector<byte> &Key, std::vector<byte> &PlainText, std::vector<byte> &MacCode)
	{
		Mac::Poly1305 gen;
		Key::Symmetric::SymmetricKey kp(Key);
		gen.Initialize(kp);
		gen.Update(PlainText, 0, PlainText.size());

		std::vector<byte> code(16);
		gen.Finalize(code, 0);

		if (MacCode != code)
		{
			throw TestException("Poly1305Compare: Tags do not match!");
		}
	}

	void Poly1305Test::Poly1305AESCompare(std::vector<byte> &Key, std::vector<byte> &Nonce, std::vector<byte> &PlainText, std::vector<byte> &MacCode)
	{
		Mac::Poly1305 gen(Enumeration::BlockCiphers::Rijndael);
		Key::Symmetric::SymmetricKey kp(Key, Nonce);
		gen.Initialize(kp);
		gen.Update(PlainText, 0, PlainText.size());

		std::vector<byte> code(16);
		gen.Finalize(code, 0);

		if (MacCode != code)
		{
			throw TestException("Poly1305AESCompare: Tags do not match!");
		}
	}

	void Poly1305Test::Initialize()
	{
		/*lint -save -e146 */
		/*lint -save -e417 */
		const std::vector<std::string> keys =
		{
			std::string("85D6BE7857556D337F4452FE42D506A80103808AFB0DB2FD4ABFF6AF4149F51B"),
			std::string("746869732069732033322D62797465206B657920666F7220506F6C7931333035"),
			std::string("0000000000000000000000000000000000000000000000000000000000000000"),
			std::string("0000000000000000000000000000000036E5F6B5C5E06070F0EFCA96227A863E"),
			std::string("36E5F6B5C5E06070F0EFCA96227A863E00000000000000000000000000000000"),
			std::string("1C9240A5EB55D38AF333888604F6B5F0473917C1402B80099DCA5CBC207075C0"),
			std::string("0200000000000000000000000000000000000000000000000000000000000000"),
			std::string("02000000000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"),
			std::string("0100000000000000000000000000000000000000000000000000000000000000"),
			std::string("0100000000000000000000000000000000000000000000000000000000000000"),
			std::string("0200000000000000000000000000000000000000000000000000000000000000"),
			std::string("0100000000000000040000000000000000000000000000000000000000000000"),
			std::string("0100000000000000040000000000000000000000000000000000000000000000"),
			std::string("EEA6A7251C1E72916D11C2CB214D3C252539121D8E234E652D651FA4C8CFF880"),
			std::string("01BCB20BFC8B6E03609DDD09F44B060F95CC0E44D0B79A8856AFCAE1BEC4FE3C"),
			std::string("CD07FD0EF8C0BE0AFCBDB30AF4AF000976FB3635A2DC92A1F768163AB12F2187"),
			std::string("0000000000000000000000000000000000000000000000000000000000000000"),
			std::string("F795BD0A50E29E0710D3130A20E98D0CF795BD4A52E29ED713D313FA20E98DBC"),
			std::string("3EF49901C8E11C000430D90AD45E7603E69DAE0AAB9F91C03A325DCC9436FA90"),
			std::string("DA4AFC035087D90E503F8F0EA08C3E0D85A4EA91A7DE0B0D96EED0D4BF6ECF1C"),
			std::string("CA3C6A0DA0A864024CA3090628C28E0D25EB69BAC5CDF7D6BFCEE4D9D5507B82")
		};
		HexConverter::Decode(keys, 21, m_key);

		const std::vector<std::string> nonce =
		{
			std::string("00000000000000000000000000000000"),
			std::string("917CF69EBD68B2EC9B9FE9A3EADDA692"),
			std::string("166450152E2394835606A9D1DD2CDC8B"),
			std::string("0B6EF7A0B8F8C738B0F8D5995415271F"),
			std::string("046772A4F0A8DE92E4F0D628CDB04484")
		};
		HexConverter::Decode(nonce, 5, m_nonce);

		const std::vector<std::string> plain =
		{
			std::string("43727970746F6772617068696320466F72756D2052657365617263682047726F7570"),
			std::string("48656C6C6F20776F726C6421"),
			std::string("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"),
			std::string("416E79207375626D697373696F6E20746F20746865204945544620696E74656E6465642062792074686520436F6E7472696275746F7220666F72207075626C69"
			"636174696F6E20617320616C6C206F722070617274206F6620616E204945544620496E7465726E65742D4472616674206F722052464320616E6420616E7920737"
			"46174656D656E74206D6164652077697468696E2074686520636F6E74657874206F6620616E204945544620616374697669747920697320636F6E736964657265"
			"6420616E20224945544620436F6E747269627574696F6E222E20537563682073746174656D656E747320696E636C756465206F72616C2073746174656D656E747"
			"320696E20494554462073657373696F6E732C2061732077656C6C206173207772697474656E20616E6420656C656374726F6E696320636F6D6D756E6963617469"
			"6F6E73206D61646520617420616E792074696D65206F7220706C6163652C207768696368206172652061646472657373656420746F"),
			std::string("416E79207375626D697373696F6E20746F20746865204945544620696E74656E6465642062792074686520436F6E7472696275746F7220666F72207075626C69"
			"636174696F6E20617320616C6C206F722070617274206F6620616E204945544620496E7465726E65742D4472616674206F722052464320616E6420616E7920737"
			"46174656D656E74206D6164652077697468696E2074686520636F6E74657874206F6620616E204945544620616374697669747920697320636F6E736964657265"
			"6420616E20224945544620436F6E747269627574696F6E222E20537563682073746174656D656E747320696E636C756465206F72616C2073746174656D656E747"
			"320696E20494554462073657373696F6E732C2061732077656C6C206173207772697474656E20616E6420656C656374726F6E696320636F6D6D756E6963617469"
				"6F6E73206D61646520617420616E792074696D65206F7220706C6163652C207768696368206172652061646472657373656420746F"),
			std::string("2754776173206272696C6C69672C20616E642074686520736C6974687920746F7665730A446964206779726520616E642067696D626C6520696E207468652077"
			"6162653A0A416C6C206D696D737920776572652074686520626F726F676F7665732C0A416E6420746865206D6F6D65207261746873206F757467726162652E"),
			std::string("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"),
			std::string("02000000000000000000000000000000"),
			std::string("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF11000000000000000000000000000000"),
			std::string("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE01010101010101010101010101010101"),
			std::string("FDFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"),
			std::string("E33594D7505E43B900000000000000003394D7505E4379CD01000000000000000000000000000000000000000000000001000000000000000000000000000000"),
			std::string("E33594D7505E43B900000000000000003394D7505E4379CD010000000000000000000000000000000000000000000000"),
			std::string("8E993B9F48681273C29650BA32FC76CE48332EA7164D96A4476FB8C531A1186AC0DFC17C98DCE87B4DA7F011EC48C97271D2C20F9B928FE2270D6FB863D51738B4"
				"8EEEE314A7CC8AB932164548E526AE90224368517ACFEABD6BB3732BC0E9DA99832B61CA01B6DE56244A9E88D5F9B37973F622A43D14A6599B1F654CB45A74E355A5"),
			std::string("66F75C0E0C7A40658629E3392F7F8E3349A02191FFD49F39879A8D9D1D0E23EA3CAA4D240BD2AB8A8C4A6BB8D3288D9DE4B793F05E97646DD4D98055DE"
				"FC3E0677D956B4C62664BAC15962AB15D93CCBBC03AAFDBDE779162ED93B55361F0F8ACAA41D50EF5175927FE79EA316186516EEF15001CD04D3524A55"
				"E4FA3C5CA479D3AAA8A897C21807F721B6270FFC68B6889D81A116799F6AAA35D8E04C7A7DD5E6DA2519E8759F54E906696F5772FEE093283BCEF7B930"
				"AED50323BCBC8C820C67422C1E16BDC022A9C0277C9D95FEF0EA4EE11E2B27276DA811523C5ACB80154989F8A67EE9E3FA30B73B0C1C34BF46E3464D97"
				"7CD7FCD0AC3B82721080BB0D9B982EE2C77FEEE983D7BA35DA88CE86955002940652AB63BC56FB16F994DA2B01D74356509D7D1B6D7956B0E5A557757B"
				"D1CED2EEF8650BC5B6D426108C1518ABCBD0BEFB6A0D5FD57A3E2DBF31458EAB63DF66613653D4BEAE73F5C40EB438FBCFDCF4A4BA46320184B9CA0DA4"
				"DFAE77DE7CCC910356CAEA3243F33A3C81B064B3B7CEDC7435C223F664227215715980E6E0BB570D459BA80D7512DBE458C8F0F3F52D659B6E8EEF19EE"
				"71AEA2CED85C7A42FFCA6522A62DB49A2A46EFF72BD7F7E0883ACD087183F0627F3537A4D558754ED63358E8182BEE196735B361DC9BD64D5E34E1074A"
				"855655D2974CC6FA1653754CF40F561D8C7DC526AAB2908EC2D2B977CDE1A1FB1071E32F40E049EA20F30368BA1592B4FE57FB51595D23ACBDACE324CD"
				"D78060A17187C662368854E915402D9B52FB21E984663E41C26A109437E162CFAF071B53F77E50000A5388FF183B82CE7A1AF476C416D7D204157B3633"
				"B2F4EC077B699B032816997E37BCEDED8D4A04976FD7D0C0B029F290794C3BE504C5242287EA2F831F11ED5690D92775CD6E863D7731FD4DA687EBFB13"
				"DF4C41DC0FB8"),
			std::string("F05204A74F0F88A7FA1A95B84EC3D8FFB36FCDC7723EA65DFE7CD464E86E0ABF6B9D51DB3220CFD8496AD6E6D36EBEE8D990F9CE0D3BB7F72B7AB5B3AB0A7"
				"3240D11EFE772C857021AE859DB4933CDDE4387B471D2CE700FEF4B81087F8F47C307881FD83017AFCD15B8D21EDF9B704677F46DF97B07E5B83F87C8A"
				"BD90AF9B1D0F9E2710E8EBD0D4D1C6A055ABEA861F42368BED94D9373E909C1D3715B221C16BC524C55C31EC3EAB204850BB2474A84F9917038EFF9D92"
				"1130951391B5C54F09B5E1DE833EA2CD7D3B306740ABB7096D1E173DA83427DA2ADDDD3631EDA30B54DBF487F2B082E8646F07D6E0A87E97522CA38D4A"
				"CE4954BF3DB6DD3A93B06FA18EB56856627ED6CFFCD7AE26374554CA18AB8905F26331D323FE10E6E70624C7BC07A70F06ECD804B48F8F7E75E910165E"
				"1BEB554F1F0EC7949C9C8D429A206B4D5C0653102249B6098E6B45FAC2A07FF0220B0B8AE8F4C6BCC0C813A7CD141FA8B398B42575FC395747C5A0257A"
				"C41D6C1F434CFBF5DFE8349F5347EF6B60E611F5D6C3CBC20CA2555274D1934325824CEF4809DA293EA13F181929E2AF025BBD1C9ABDC3AF93AFD4C50A"
				"2854ADE3887F4D2C8C225168052C16E74D76D2DD3E9467A2C5B8E15C06FFBFFA42B8536384139F07E195A8C9F70F514F31DCA4EB2CF262C0DCBDE53654"
				"B6250A29EFE21D54E83C80E005A1CAD36D5934FF01C32E4BC5FE06D03064FF4A268517DF4A94C759289F323734318CFA5D859D4CE9C16E63D02DFF0896"
				"976F521607638535D2EE8DD3312E1DDC80A55D34FE829AB954C1EBD54D929954770F1BE9D32B4C05003C5C9E97943B6431E2AFE820B1E967B19843E598"
				"5A131B1100517CDC363799104AF91E2CF3F53CB8FD003653A6DD8A31A3F9D566A7124B0FFE9695BCB87C482EB60106F88198F766A40BC0F4873C23653C"
				"5F9E7A8E446F770BEB8034CF01D21028BA15CCEE21A8DB918C4829D61C88BFA927BC5DEF831501796C5B401A60A6B1B433C9FB905C8CD40412FFFEE81AB"),
			std::string(""),
			std::string("66F7"),
			std::string("66F75C0E0C7A406586"),
			std::string("66F75C0E0C7A40658629E3392F7F8E3349A02191FFD49F39879A8D9D1D0E23EA"),
			std::string("66F75C0E0C7A40658629E3392F7F8E3349A02191FFD49F39879A8D9D1D0E23EA3CAA4D240BD2AB8A8C4A6BB8D3288D9DE4B793F05E97646DD4D98055DE")

		};
		HexConverter::Decode(plain, 21, m_plainText);

		const std::vector<std::string> code =
		{
			std::string("A8061DC1305136C6C22B8BAF0C0127A9"),
			std::string("A6F745008F81C916A20DCC74EEF2B2F0"),
			std::string("00000000000000000000000000000000"),
			std::string("36E5F6B5C5E06070F0EFCA96227A863E"),
			std::string("F3477E7CD95417AF89A6B8794C310CF0"),
			std::string("4541669A7EAAEE61E708DC7CBCC5EB62"),
			std::string("03000000000000000000000000000000"),
			std::string("03000000000000000000000000000000"),
			std::string("05000000000000000000000000000000"),
			std::string("00000000000000000000000000000000"),
			std::string("FAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"),
			std::string("14000000000000005500000000000000"),
			std::string("13000000000000000000000000000000"),
			std::string("F3FFC7703F9400E52A7DFB4B3D3305D9"),
			std::string("AE345D555EB04D6947BB95C0965237E2"),
			std::string("045BE28CC52009F506BDBFABEDACF0B4"),
			std::string("66E94BD4EF8A2C3B884CFA59CA342B2E"),
			std::string("5CA585C75E8F8F025E710CABC9A1508B"),
			std::string("2924F51B9C2EFF5DF09DB61DD03A9CA1"),
			std::string("3C5A13ADB18D31C64CC29972030C917D"),
			std::string("FC5FB58DC65DAF19B14D1D05DA1064E8")
		};
		HexConverter::Decode(code, 21, m_expectedCode);
		/*lint -restore */
	}

	void Poly1305Test::OnProgress(std::string Data)
	{
		m_progressEvent(Data);
	}
}
