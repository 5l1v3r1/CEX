#include "CFES.h"
#include "ExampleUtils.h"
#include "FileTools.h"
#include "../../CEX/ACP.h"
#include "../../CEX/HBA.h"
#include "../../CEX/RCS.h"

namespace Example
{
	using namespace CEX;
	using Provider::ACP;
	using Enumeration::BlockCiphers;
	using Enumeration::CipherModes;
	using Exception::CryptoAuthenticationFailure;
	using Cipher::Block::Mode::HBA;
	using Cipher::Stream::RCS;
	using Enumeration::StreamAuthenticators;
	using Cipher::SymmetricKey;

	const std::string CFES::CFES_ENCRYPT_EXTENSION = ".cenc";
	const std::string CFES::CFES_KEY_EXTENSION = ".ckey";
	const std::string CEFS_COMMAND_PROMPT = "CFES> ";
	const size_t CEFS_MENU_SIZE = 34;

	enum MessageIndex : size_t
	{
		CEFS_ENC_CREATED = 0,
		CEFS_ENC_ABORT = 1,
		CEFS_ENC_SUCCESS = 2,
		CEFS_ENC_FAIL = 3,
		CEFS_KEY_ABORT = 4,
		CEFS_SES_CANCELLED = 5,
		CEFS_KEY_DETECTED = 6,
		CEFS_DEC_PERM = 7,
		CEFS_DEC_SUCCESS = 8,
		CEFS_DEC_FAIL = 9,
		CEFS_DEC_ABORT = 10,
		CEFS_DEC_CANCELLED = 11,
		CEFS_TITLE_LINE1 = 12,
		CEFS_TITLE_LINE2 = 13,
		CEFS_TITLE_LINE3 = 14,
		CEFS_HELP_LINE1 = 15,
		CEFS_HELP_LINE2 = 16,
		CEFS_HELP_LINE3 = 17,
		CEFS_MENU_LINE1 = 18,
		CEFS_MENU_LINE2 = 19,
		CEFS_MENU_LINE3 = 20,
		CEFS_MENU_LINE4 = 21,
		CEFS_MENU_LINE5 = 22,
		CEFS_MENU_LINE6 = 23,
		CEFS_MENU_LINE7 = 24,
		CEFS_MENU_LINE8 = 25,
		CEFS_MENU_LINE9 = 26,
		CEFS_MENU_LINE10 = 27,
		CEFS_MENU_LINE11 = 28,
		CEFS_MENU_LINE12 = 29,
		CEFS_MENU_LINE13 = 30,
		CEFS_MENU_LINE14 = 31,
		CEFS_MENU_LINE15 = 32,
		CEFS_MENU_LINE16 = 33,
		CEFS_EMPTY_LINE = 99
	};
	
	std::vector<std::string> CFES::MessageStrings =
	{
		// english
		std::string("CFES> The key-file has been created at:"),
		std::string("CFES> The encrypted file could not be created, operation aborted."),
		std::string("CFES> The encrypted file has been created at:"),
		std::string("CFES> The file could not be written, check path and directory permissions."),
		std::string("CFES> The key file could not be created, operation aborted."),
		std::string("CFES> Session cancelled, the user aborted the operation."),
		std::string("CFES> The encryption key file was not detected."),
		std::string("CFES> The file could not be written, check path and directory permissions."),
		std::string("CFES> The decrypted file has been created at:"),
		std::string("CFES> The file could not be decrypted, key or file may be damaged."),
		std::string("CFES> The file could not be written, check path and directory permissions."),
		std::string("CFES> Session cancelled, the user aborted the operation."),
		std::string("CFES is a Post Quantum Secure file encryption service."),
		std::string("It uses powerful new symmetric ciphers to encrypt and authenticate a file."),
		std::string("Follow the menus to create a key and encrypt a file, or authenticate and decrypt a file."),
		std::string("Usage: add the full path to a file when prompted; if the file has a .ckey extension, \ndecryption mode is selected, otherwise the cipher is initialized for encryption."),
		std::string("In encryption mode, a key is generated that uses the file-name and the .ckey extension, \nand an encrypted copy of the file is created with the .cenc extension in the originating directory."),
		std::string("To decrypt a file, put the .ckey and .cenc files in the same directory, \nor specify the full path to the key when prompted."),
		std::string("CFES> The file exists. Press Y and enter to delete existing file, or N and enter to abort."),
		std::string("CFES> Select the cipher and mode of encryption:"),
		std::string("CFES> 0) Cancel the operation"),
		std::string("CFES> 1) HBA-RHX-256 Authenticated mode"),
		std::string("CFES> 2) HBA-RHX-512 Authenticated mode"),
		std::string("CFES> 3) RCS-256 Authenticated stream cipher"),
		std::string("CFES> 4) RCS-512 Authenticated stream cipher"),
		std::string("CFES> Make a selection and press enter to proceed"),
		std::string("CFES> Enter the full path to a file, or an empty line to cancel, and press enter"),
		std::string("CFES> Enter the full path to the key file, or an empty line to cancel, and press enter"),
		std::string("Select from the following menu options:"),
		std::string("0) Cancel the operation"),
		std::string("1) Encrypt a file and output the key"),
		std::string("2) Input a key and Decrypt a file"),
		std::string("Make a selection and press enter to proceed"),
		std::string("Delete"),
		// french
		std::string("CFES> Le fichier de cl�s a �t� cr�� �:"),
		std::string("CFES> Le fichier chiffr� n'a pas pu �tre cr��, op�ration abandonn�e."),
		std::string("CFES> Le fichier crypt� a �t� cr�� �:"),
		std::string("CFES> Le fichier n'a pas pu �tre �crit, v�rifiez les autorisations de chemin et de r�pertoire."),
		std::string("CFES> Le fichier de cl� n'a pas pu �tre cr��, op�ration abandonn�e."),
		std::string("CFES> Session annul�e, l'utilisateur a abandonn� l'op�ration."),
		std::string("CFES> Le fichier de cl� de chiffrement n'a pas �t� d�tect�."),
		std::string("CFES> Le fichier n'a pas pu �tre �crit, v�rifiez les autorisations de chemin et de r�pertoire."),
		std::string("CFES> Le fichier d�chiffr� a �t� cr�� �:"),
		std::string("CFES> Le fichier n'a pas pu �tre d�chiffr�, la cl� ou le fichier peut �tre endommag�."),
		std::string("CFES> Le fichier n'a pas pu �tre �crit, v�rifiez les autorisations de chemin et de r�pertoire."),
		std::string("CFES> Session annul�e, l'utilisateur a abandonn� l'op�ration."),
		std::string("CFES est un service de chiffrement de fichiers Post Quantum Secure."),
		std::string("Il utilise de nouveaux chiffrements sym�triques puissants pour crypter et authentifier un fichier."),
		std::string("Suivez les menus pour cr�er une cl� et crypter un fichier, ou authentifier et d�crypter un fichier."),
		std::string("Utilisation: ajoutez le chemin d'acc�s complet � un fichier lorsque vous y �tes invit�; \nsi le fichier a une extension .ckey, le mode de d�chiffrement est s�lectionn�, sinon le chiffrement est initialis� pour le chiffrement."),
		std::string("En mode de chiffrement, une cl� est g�n�r�e qui utilise le nom de fichier et l'extension .ckey, \net une copie chiffr�e du fichier est cr��e avec l'extension .cenc dans le r�pertoire d'origine."),
		std::string("Pour d�crypter un fichier, placez les fichiers .ckey et .cenc dans le m�me r�pertoire ou \nsp�cifiez le chemin d'acc�s complet � la cl� lorsque vous y �tes invit�."),
		std::string("CFES> The file exists. Press Y and enter to delete existing file, or N and enter to abort."),
		std::string("CFES> S�lectionnez le chiffrement et le mode de chiffrement:"),
		std::string("CFES> 0) Annuler l'op�ration"),
		std::string("CFES> 1) HBA-RHX-256 Mode authentifi�"),
		std::string("CFES> 2) HBA-RHX-512 Mode authentifi�"),
		std::string("CFES> 3) RCS-256 Chiffrement de flux authentifi�"),
		std::string("CFES> 4) RCS-512 Chiffrement de flux authentifi�"),
		std::string("CFES> Faites une s�lection et appuyez sur Entr�e pour continuer"),
		std::string("CFES> Entrez le chemin d'acc�s complet � un fichier ou une ligne vide pour annuler et appuyez sur Entr�e"),
		std::string("CFES> Entrez le chemin d'acc�s complet au fichier cl� ou une ligne vide pour annuler, puis appuyez sur Entr�e"),
		std::string("S�lectionnez parmi les options de menu suivantes:"),
		std::string("0) Annuler l'op�ration"),
		std::string("1) Chiffrer un fichier et sortir la cl�"),
		std::string("2) Saisissez une cl� et d�cryptez un fichier"),
		std::string("Faites une s�lection et appuyez sur Entr�e pour continuer"),
		std::string("Supprimer"),
		// spanish
		std::string("CFES> El archivo clave se ha creado en:"),
		std::string("CFES> No se pudo crear el archivo encriptado, se cancel� la operaci�n."),
		std::string("CFES> El archivo cifrado se ha creado en:"),
		std::string("CFES> No se pudo escribir el archivo, verifique la ruta y los permisos del directorio."),
		std::string("CFES> No se pudo crear el archivo de clave, se cancel� la operaci�n."),
		std::string("CFES> Sesi�n cancelada, el usuario abort� la operaci�n."),
		std::string("CFES> No se detect� el archivo de clave de cifrado."),
		std::string("CFES> No se pudo escribir el archivo, verifique la ruta y los permisos del directorio."),
		std::string("CFES> El archivo descifrado se ha creado en:"),
		std::string("CFES> No se pudo descifrar el archivo, la clave o el archivo pueden estar da�ados."),
		std::string("CFES> No se pudo escribir el archivo, verifique la ruta y los permisos del directorio."),
		std::string("CFES> Sesi�n cancelada, el usuario abort� la operaci�n."),
		std::string("CFES es un servicio de cifrado de archivos Post Quantum Secure."),
		std::string("Utiliza nuevos y potentes cifrados sim�tricos para cifrar y autenticar un archivo."),
		std::string("Siga los men�s para crear una clave y cifrar un archivo, o autenticar y descifrar un archivo."),
		std::string("Uso: agregue la ruta completa a un archivo cuando se le solicite; si el archivo tiene una extensi�n .ckey, se selecciona el modo de descifrado; de lo contrario, el cifrado se inicializa para el cifrado."),
		std::string("En el modo de cifrado, se genera una clave que utiliza el nombre de archivo y la extensi�n .ckey, y se crea una copia cifrada del archivo con la extensi�n .cenc en el directorio de origen."),
		std::string("Para descifrar un archivo, coloque los archivos .ckey y .cenc en el mismo directorio, o especifique la ruta completa a la clave cuando se le solicite."),
		std::string("CFES> El archivo existe. Presione Y e ingrese para eliminar el archivo existente, o N e ingrese para cancelar."),
		std::string("CFES> Seleccione el cifrado y el modo de cifrado:"),
		std::string("CFES> 0) Cancelar la operaci�n"),
		std::string("CFES> 1) Modo autenticado HBA-RHX-256"),
		std::string("CFES> 2) Modo autenticado HBA-RHX-512"),
		std::string("CFES> 3) Cifrado de flujo autenticado RCS-256"),
		std::string("CFES> 4) Cifrado de flujo autenticado RCS-512"),
		std::string("CFES> Seleccione y presione Intro para continuar"),
		std::string("CFES> Ingrese la ruta completa a un archivo, o una l�nea vac�a para cancelar, y presione enter"),
		std::string("CFES> Ingrese la ruta completa al archivo de clave, o una l�nea vac�a para cancelar, y presione enter"),
		std::string("Seleccione entre las siguientes opciones de men�:"),
		std::string("0) Cancelar la operaci�n"),
		std::string("1) Cifre un archivo y env�e la clave"),
		std::string("2) Ingrese una clave y descifre un archivo"),
		std::string("Haga una selecci�n y presione enter para continuar"),
		std::string("Eliminar"),
		// german
		std::string("CFES> Die Schl�sseldatei wurde erstellt unter:"),
		std::string("CFES> Die verschl�sselte Datei konnte nicht erstellt werden, Vorgang abgebrochen."),
		std::string("CFES> Die verschl�sselte Datei wurde erstellt um:"),
		std::string("CFES> Die Datei konnte nicht geschrieben werden. �berpr�fen Sie die Pfad- und Verzeichnisberechtigungen"),
		std::string("CFES> Die Schl�sseldatei konnte nicht erstellt werden, Vorgang abgebrochen."),
		std::string("CFES> Sitzung abgebrochen, der Benutzer hat den Vorgang abgebrochen."),
		std::string("CFES> Die Verschl�sselungsschl�sseldatei wurde nicht erkannt."),
		std::string("CFES> Die Datei konnte nicht geschrieben werden. �berpr�fen Sie die Pfad- und Verzeichnisberechtigungen."),
		std::string("CFES> Die entschl�sselte Datei wurde erstellt um:"),
		std::string("CFES> Die Datei konnte nicht entschl�sselt werden, der Schl�ssel oder die Datei sind m�glicherweise besch�digt."),
		std::string("CFES> Die Datei konnte nicht geschrieben werden. �berpr�fen Sie die Pfad- und Verzeichnisberechtigungen."),
		std::string("CFES> Sitzung abgebrochen, der Benutzer hat den Vorgang abgebrochen."),
		std::string("CFES ist ein Post Quantum Secure-Dateiverschl�sselungsdienst."),
		std::string("Es verwendet leistungsstarke neue symmetrische Chiffren, um eine Datei zu verschl�sseln und zu authentifizieren."),
		std::string("Befolgen Sie die Men�s, um einen Schl�ssel zu erstellen und eine Datei zu verschl�sseln oder eine Datei zu authentifizieren und zu entschl�sseln."),
		std::string("Verwendung: F�gen Sie den vollst�ndigen Pfad zu einer Datei hinzu, wenn Sie dazu aufgefordert werden. Hat die Datei die Erweiterung .ckey, wird der Entschl�sselungsmodus ausgew�hlt, andernfalls wird die Verschl�sselung f�r die Verschl�sselung initialisiert."),
		std::string("Im Verschl�sselungsmodus wird ein Schl�ssel generiert, der den Dateinamen und die Erweiterung .ckey verwendet, und eine verschl�sselte Kopie der Datei wird mit der Erweiterung .cenc im Ursprungsverzeichnis erstellt."),
		std::string("Um eine Datei zu entschl�sseln, legen Sie die Dateien .ckey und .cenc im selben Verzeichnis ab oder geben Sie den vollst�ndigen Pfad zum Schl�ssel an, wenn Sie dazu aufgefordert werden."),
		std::string("CFES> Die Datei existiert. Dr�cken Sie J und die Eingabetaste, um eine vorhandene Datei zu l�schen, oder N und die Eingabetaste, um den Vorgang abzubrechen."),
		std::string("CFES> W�hlen Sie die Verschl�sselung und den Verschl�sselungsmodus aus:"),
		std::string("CFES> 0) Vorgang abbrechen "),
		std::string("> 1) HBA-RHX-256 Authentifizierter Modus"),
		std::string("CFES> 2) HBA-RHX-512 Authentifizierter Modus"),
		std::string("CFES> 3) RCS-256 Authentifizierte Stream-Verschl�sselung"),
		std::string("CFES> 4) RCS-512 Authentifizierte Stream-Verschl�sselung"),
		std::string("CFES> Treffen Sie eine Auswahl und dr�cken Sie die Eingabetaste, um fortzufahren"),
		std::string("CFES> Geben Sie den vollst�ndigen Pfad zu einer Datei oder eine leere Zeile zum Abbrechen ein und dr�cken Sie die Eingabetaste"),
		std::string("CFES> Geben Sie den vollst�ndigen Pfad zur Schl�sseldatei oder eine leere Zeile zum Abbrechen ein und dr�cken Sie die Eingabetaste"),
		std::string("W�hlen Sie aus den folgenden Men�optionen:"),
		std::string("0) Brechen Sie den Vorgang ab"),
		std::string("1) Verschl�sseln Sie eine Datei und geben Sie den Schl�ssel aus"),
		std::string("2) Geben Sie einen Schl�ssel ein und entschl�sseln Sie eine Datei"),
		std::string("Treffen Sie eine Auswahl und dr�cken Sie die Eingabetaste, um fortzufahren"),
		std::string("L�schen"),
		// portuguese
		std::string("CFES> O arquivo-chave foi criado em:"),
		std::string("CFES> O arquivo criptografado n�o p�de ser criado, a opera��o foi interrompida."),
		std::string("CFES> O arquivo criptografado foi criado em: "),
		std::string("CFES> O arquivo n�o p�de ser gravado, verifique as permiss�es de caminho e diret�rio."),
		std::string("CFES> O arquivo de chave n�o p�de ser criado, a opera��o foi interrompida."),
		std::string("CFES> Sess�o cancelada, o usu�rio cancelou a opera��o."),
		std::string("CFES> O arquivo da chave de criptografia n�o foi detectado."),
		std::string("CFES> O arquivo n�o p�de ser gravado, verifique as permiss�es de caminho e diret�rio."),
		std::string("CFES> O arquivo descriptografado foi criado em:"),
		std::string("CFES> O arquivo n�o p�de ser descriptografado, a chave ou o arquivo pode estar danificado."),
		std::string("CFES> O arquivo n�o p�de ser gravado, verifique as permiss�es de caminho e diret�rio."),
		std::string("CFES> Sess�o cancelada, o usu�rio cancelou a opera��o."),
		std::string("CFES � um servi�o de criptografia de arquivos Post Quantum Secure."),
		std::string("Ele usa novas cifras sim�tricas poderosas para criptografar e autenticar um arquivo."),
		std::string("Siga os menus para criar uma chave e criptografar um arquivo ou autenticar e descriptografar um arquivo."),
		std::string("Uso: adicione o caminho completo a um arquivo quando solicitado; se o arquivo tiver uma extens�o .ckey, o modo de descriptografia ser� selecionado; caso contr�rio, a cifra ser� inicializada para criptografia."),
		std::string("No modo de criptografia, � gerada uma chave que usa o nome do arquivo e a extens�o .ckey, e uma c�pia criptografada do arquivo � criada com a extens�o .cenc no diret�rio de origem."),
		std::string("Para descriptografar um arquivo, coloque os arquivos .ckey e .cenc no mesmo diret�rio ou especifique o caminho completo para a chave quando solicitado."),
		std::string("CFES> O arquivo existe. Pressione Y e digite para excluir o arquivo existente ou N e digite para abortar."),
		std::string("CFES> Selecione a cifra e o modo de criptografia:"),
		std::string("CFES> 0) Cancelar a opera��o "),
		std::string("CFES> 1) Modo autenticado HBA-RHX-256"),
		std::string("CFES> 2) Modo autenticado HBA-RHX-512"),
		std::string("CFES> 3) Cifra de fluxo autenticada RCS-256"),
		std::string("CFES> 4) Cifra de fluxo autenticada RCS-512"),
		std::string("CFES> Fa�a uma sele��o e pressione Enter para continuar"),
		std::string("CFES> Digite o caminho completo para um arquivo ou uma linha vazia para cancelar e pressione "),
		std::string("CFES> Digite o caminho completo para o arquivo de chave ou uma linha vazia para cancelar e pressione enter"),
		std::string("Selecione entre as seguintes op��es de menu:"),
		std::string("0) Cancele a opera��o"),
		std::string("1) Criptografar um arquivo e gerar a chave"),
		std::string(") Insira uma chave e descriptografar um arquivo"),
		std::string("Fa�a uma sele��o e pressione enter para continuar"),
		std::string("Excluir")
	};

	class CFES::CFESState
	{
	public:

		SecureVector<byte> key;
		SecureVector<byte> nonce;
		int32_t cmode;
		BlockCiphers bcpr;
		StreamAuthenticators sauth;

		CFESState()
			:
			key(0),
			nonce(0),
			cmode(0),
			bcpr(BlockCiphers::None),
			sauth(StreamAuthenticators::None)
		{};

		~CFESState()
		{
			SecureClear(key);
			SecureClear(nonce);
			cmode = 0;
			bcpr = BlockCiphers::None;
			sauth = StreamAuthenticators::None;
		}
	};

	//~~~Public Functions~~~//

	void CFES::Run()
	{
		int32_t cmode;
		std::string fpath;
		std::string kpath;
		bool encrypt;
		bool res;
		CFESState state;

		fpath = MenuFilePath();
		res = (fpath.size() != 0 && FileTools::Exists(fpath));

		if (res == true)
		{
			encrypt = (ExampleUtils::StringContains(fpath, CFES_ENCRYPT_EXTENSION) == false);
			kpath = FileTools::Path(fpath) + FileTools::Name(fpath) + CFES_KEY_EXTENSION;

			if (encrypt == true)
			{
				cmode = MenuCipherMode();

				if (cmode != 0)
				{
					// initialize the state
					LoadCipherState(state, cmode);

					if (FileTools::Exists(kpath))
					{
						res = MenuDeleteFile(kpath);

						if (res == true)
						{
							res = FileTools::Delete(kpath);
						}
					}

					if (res == true)
					{
						// create the key file
						res = FileTools::Create(kpath);

						// sk = k+n + (ext+cprm+ts)
						std::string ext = FileTools::Extension(fpath);
						const size_t HDRLEN = ext.size() + 2;
						std::string epath;
						SecureVector<byte> tmpr(state.key.size() + state.nonce.size() + HDRLEN);

						// generate the random key and add it to the key array
						SecureGenerate(tmpr, 0, state.key.size() + state.nonce.size());
						MemoryTools::Copy(tmpr, 0, state.key, 0, state.key.size());
						MemoryTools::Copy(tmpr, state.key.size(), state.nonce, 0, state.nonce.size());

						// add the file extension, the cipher code, and the total size to the end of the key-file
						tmpr[tmpr.size() - 1] = static_cast<byte>(HDRLEN);
						tmpr[tmpr.size() - 2] = static_cast<byte>(cmode);
						MemoryTools::Copy(ext, 0, tmpr, state.key.size() + state.nonce.size(), ext.size());

						// create the key file and write the contents
						res = FileTools::Create(kpath);

						if (res == true)
						{
							FileTools::Write(kpath, SecureUnlock(tmpr));
							// notify the user that key has been created
							PrintMessage(CEFS_ENC_CREATED);
							ExampleUtils::WriteLine(kpath);

							// the encrypted file is the file name and path with the .cenc extension
							epath = FileTools::Path(fpath) + FileTools::Name(fpath) + CFES_ENCRYPT_EXTENSION;

							if (FileTools::Exists(epath))
							{
								res = MenuDeleteFile(epath);

								if (res == true)
								{
									res = FileTools::Delete(epath);
								}
							}

							if (res == true)
							{
								res = FileTools::Create(epath);

								// encrypt the file contents and write to new file
								if (cmode == 1)
								{
									// HBA
									state.bcpr = BlockCiphers::RHXH256;
									res = HBATransform(fpath, epath, state, encrypt);
								}
								else if (cmode == 2)
								{
									state.bcpr = BlockCiphers::RHXH512;
									res = HBATransform(fpath, epath, state, encrypt);
								}
								else
								{
									// RCS
									res = RCSTransform(fpath, epath, state, encrypt);
								}
							}
							else
							{
								PrintMessage(CEFS_ENC_ABORT);
							}
						}

						if (res == true)
						{
							// notify user that file has been written successfully
							PrintMessage(CEFS_ENC_SUCCESS);
							ExampleUtils::WriteLine(epath);
						}
						else
						{
							PrintMessage(CEFS_ENC_FAIL);
						}
					}
					else
					{
						PrintMessage(CEFS_KEY_ABORT);
					}
				}
				else
				{
				PrintMessage(CEFS_SES_CANCELLED);
				}
			}
			else
			{
				// decrypt

				if (FileTools::Exists(kpath) == false)
				{
					PrintMessage(CEFS_KEY_DETECTED);
					kpath = MenuKeyLoad();
				}

				if (kpath.size() != 0 && FileTools::Exists(kpath) == true)
				{
					const size_t FLELEN = FileTools::Size(kpath);
					std::vector<byte> tmpr(FLELEN);
					std::string dpath;
					std::string ext;
					CFESState state;
					int32_t cmode;

					FileTools::Read(kpath, tmpr);

					// parse the footer size, cipher type, and extension
					const size_t HDRLEN = tmpr[tmpr.size() - 1];
					const size_t EXTLEN = HDRLEN - 2;
					cmode = tmpr[tmpr.size() - 2];
					ext.resize(EXTLEN);

					LoadCipherState(state, cmode);
					// copy the key and nonce to state
					MemoryTools::Copy(tmpr, 0, state.key, 0, state.key.size());
					MemoryTools::Copy(tmpr, state.key.size(), state.nonce, 0, state.nonce.size());
					MemoryTools::CopyToObject(tmpr, state.key.size() + state.nonce.size(), (char*)ext.data(), EXTLEN);

					// the decrypted file path and name
					dpath = FileTools::Path(fpath) + FileTools::Name(fpath) + std::string(".") + ext;

					// file exists, delete or abort
					if (FileTools::Exists(dpath) == true)
					{
						res = MenuDeleteFile(dpath);

						if (res == true)
						{
							res = FileTools::Delete(dpath);
						}
					}

					if (res == true)
					{
						// create the decrypted file
						res = FileTools::Create(dpath);

						// decrypt the data to the output file
						if (res == true)
						{
							if (cmode == 1)
							{
								// HBA
								state.bcpr = BlockCiphers::RHXH256;
								res = HBATransform(fpath, dpath, state, encrypt);
							}
							else if (cmode == 2)
							{
								state.bcpr = BlockCiphers::RHXH512;
								res = HBATransform(fpath, dpath, state, encrypt);
							}
							else
							{
								// RCS
								res = RCSTransform(fpath, dpath, state, encrypt);
							}

							if (res == true)
							{
								// notify user that file has been written successfully
								PrintMessage(CEFS_DEC_SUCCESS);
								ExampleUtils::WriteLine(dpath);
							}
							else
							{
								PrintMessage(CEFS_DEC_FAIL);
							}
						}
						else
						{
							PrintMessage(CEFS_DEC_PERM);
						}
					}
					else
					{
						PrintMessage(CEFS_DEC_PERM);
					}
				}
				else
				{
					PrintMessage(CEFS_DEC_CANCELLED);
				}
			}
		}
	}

	void CFES::Help()
	{
		PrintMessage(CEFS_TITLE_LINE1);
		PrintMessage(CEFS_TITLE_LINE2);
		PrintMessage(CEFS_TITLE_LINE3);
		PrintMessage(CEFS_EMPTY_LINE);
		PrintMessage(CEFS_HELP_LINE1);
		PrintMessage(CEFS_HELP_LINE2);
		PrintMessage(CEFS_HELP_LINE3);
	}

	void CFES::PrintTitle()
	{
		ExampleUtils::WriteLine("CFES - CEX File Encryption Service");
		ExampleUtils::WriteLine("Version 1.0a");
		ExampleUtils::WriteLine("January 12, 2020");
		ExampleUtils::WriteLine("CEX++ -Digital Freedom Defence-");
		PrintMessage(CEFS_EMPTY_LINE);
	}

	//~~~Private Functions~~~//

	bool CFES::LoadCipherState(CFESState &State, int32_t CMode)
	{
		bool res;

		switch (CMode)
		{
		case 1:
		{
			State.sauth = StreamAuthenticators::HMACSHA256;
			State.key.resize(32);
			State.nonce.resize(16);
			State.cmode = 1;
			res = true;
			break;
		}
		case 2:
		{
			State.sauth = StreamAuthenticators::HMACSHA512;
			State.key.resize(64);
			State.nonce.resize(16);
			State.cmode = 2;
			res = true;
			break;
		}
		case 3:
		{
			State.sauth = StreamAuthenticators::KMAC256;
			State.key.resize(32);
			State.nonce.resize(32);
			State.cmode = 3;
			res = true;
			break;
		}
		case 4:
		{
			State.sauth = StreamAuthenticators::KMAC512;
			State.key.resize(64);
			State.nonce.resize(32);
			State.cmode = 4;
			res = true;
			break;
		}
		default:
		{
			State.cmode = 0;
			res = false;
		}
		}

		return res;
	}

	bool CFES::HBATransform(const std::string &InputFile, const std::string &OutputFile, CFESState &State, bool Encryption)
	{
		// HBA mode
		std::vector<byte> input(0);
		std::vector<byte> output(0);
		const size_t INPLEN = FileTools::Size(InputFile);
		bool res;

		res = false;

		// initialize and key the cipher
		HBA cpr(State.bcpr, State.sauth);
		SymmetricKey kp(State.key, State.nonce);
		cpr.Initialize(Encryption, kp);

		if (Encryption)
		{
			// read from file and size output
			input.resize(INPLEN);
			output.resize(INPLEN + cpr.TagSize());
			FileTools::Read(InputFile, input);

			// encrypt the input plain-text
			cpr.Transform(input, 0, output, 0, input.size());
			FileTools::Write(OutputFile, output);
			res = true;
		}
		else
		{
			input.resize(INPLEN);
			output.resize(INPLEN - cpr.TagSize());
			FileTools::Read(InputFile, input);

			// if authentication fails during transform, and authentication failure exception is thrown,
			// the cipher-text is not decrypted, and this function returns false
			try
			{
				// decrypt the input cipher-text
				cpr.Transform(input, 0, output, 0, output.size());
				FileTools::Write(OutputFile, output);
				res = true;
			}
			catch (const CryptoAuthenticationFailure &)
			{
			}
		}

		return res;
	}

	size_t CFES::LanguageIndex()
	{
		std::string lng;
		size_t idx;

		lng = ExampleUtils::GetLanguage();

		// e=0,f=1,s=2,g=3,p=4 * CEFS_MENU_SIZE
		if (lng.empty())
		{
			idx = 0;
		}
		else if (lng.find("EN") != std::string::npos)
		{
			idx = 0;
		}
		else if (lng.find("FR") != std::string::npos)
		{
			idx = CEFS_MENU_SIZE;
		}
		else if (lng.find("ES") != std::string::npos)
		{
			idx = CEFS_MENU_SIZE * 2;
		}
		else if (lng.find("DE") != std::string::npos)
		{
			idx = CEFS_MENU_SIZE * 3;
		}
		else if (lng.find("PT") != std::string::npos)
		{
			idx = idx = CEFS_MENU_SIZE * 4;
		}
		else
		{
			idx = 0;
		}

		return idx;
	}

	int32_t CFES::MenuCipherMode()
	{
		std::string rbuf;
		int32_t res;

		while (true)
		{
			PrintMessage(CEFS_MENU_LINE1);
			PrintMessage(CEFS_MENU_LINE2);
			PrintMessage(CEFS_EMPTY_LINE);
			PrintMessage(CEFS_MENU_LINE3);
			PrintMessage(CEFS_MENU_LINE4);
			PrintMessage(CEFS_MENU_LINE5);
			PrintMessage(CEFS_MENU_LINE6);
			PrintMessage(CEFS_MENU_LINE7);
			PrintMessage(CEFS_EMPTY_LINE);
			PrintMessage(CEFS_MENU_LINE8);

			rbuf = ExampleUtils::GetResponse();
			PrintMessage(CEFS_EMPTY_LINE);

			if (rbuf == "0" || rbuf == "1" || rbuf == "2" || rbuf == "3" || rbuf == "4")
			{
				break;
			}
		};

		res = std::stoi(rbuf);

		return res;
	}

	bool CFES::MenuDeleteFile(std::string &FilePath)
	{
		std::string bres;
		bool ret;

		ret = false;

		while (true)
		{
			ExampleUtils::WriteLine(MessageStrings[CEFS_MENU_LINE1]);
			ExampleUtils::WriteLine(MessageStrings[CEFS_MENU_LINE16] + std::string(" ") + FilePath + std::string("?"));
			bres = ExampleUtils::GetResponse();

			if (bres == "y" || bres == "Y")
			{
				ret = true;
				break;
			}
			else if (bres == "n" || bres == "N")
			{
				break;
			}
		}

		return ret;
	}

	std::string CFES::MenuFilePath()
	{
		std::string fpath;

		while (true)
		{
			PrintMessage(CEFS_EMPTY_LINE);
			PrintMessage(CEFS_MENU_LINE9);
			fpath = ExampleUtils::GetResponse();

			if (fpath.size() > 8 && FileTools::Exists(fpath) || fpath.size() == 0)
			{
				break;
			}
		}

		return fpath;
	}

	std::string CFES::MenuKeyLoad()
	{
		std::string kpath;

		while (true)
		{
			PrintMessage(CEFS_EMPTY_LINE);
			PrintMessage(CEFS_MENU_LINE10);
			kpath = ExampleUtils::GetResponse();

			if (kpath.size() > 8 && FileTools::Exists(kpath) || kpath.size() == 0)
			{
				break;
			}
		}

		return kpath;
	}

	int32_t CFES::MenuOperation()
	{
		std::string rbuf;
		int32_t res;

		while (true)
		{
			PrintMessage(CEFS_MENU_LINE11);
			PrintMessage(CEFS_MENU_LINE12);
			PrintMessage(CEFS_MENU_LINE13);
			PrintMessage(CEFS_MENU_LINE14);
			PrintMessage(CEFS_EMPTY_LINE);
			PrintMessage(CEFS_MENU_LINE15);

			rbuf = ExampleUtils::GetResponse();
			PrintMessage(CEFS_EMPTY_LINE);

			if (rbuf == "0" || rbuf == "1" || rbuf == "2")
			{
				break;
			}
		};

		res = std::stoi(rbuf);

		return res;
	}

	void CFES::PrintMessage(size_t Index)
	{
		size_t idx;

		idx = LanguageIndex() + Index;

		if (Index == CEFS_EMPTY_LINE)
		{
			ExampleUtils::WriteLine("");
		}
		else
		{
			ExampleUtils::WriteLine(MessageStrings[idx]);
		}
	}

	bool CFES::RCSTransform(const std::string &InputFile, const std::string &OutputFile, CFESState &State, bool Encryption)
	{
		// HBA mode
		std::vector<byte> input(0);
		std::vector<byte> output(0);
		const size_t INPLEN = FileTools::Size(InputFile);
		bool res;

		res = false;

		// initialize and key the cipher
		RCS cpr(State.sauth);
		SymmetricKey kp(State.key, State.nonce);
		cpr.Initialize(Encryption, kp);

		if (Encryption)
		{
			// read from file and size output
			input.resize(INPLEN);
			output.resize(INPLEN + cpr.TagSize());
			FileTools::Read(InputFile, input);

			// encrypt the input plain-text
			cpr.Transform(input, 0, output, 0, input.size());
			FileTools::Write(OutputFile, output);
			res = true;
		}
		else
		{
			input.resize(INPLEN);
			output.resize(INPLEN - cpr.TagSize());
			FileTools::Read(InputFile, input);

			// if authentication fails during transform, and authentication failure exception is thrown,
			// the cipher-text is not decrypted, and this function returns false
			try
			{
				// decrypt the input cipher-text
				cpr.Transform(input, 0, output, 0, output.size());
				FileTools::Write(OutputFile, output);
				res = true;
			}
			catch (const CryptoAuthenticationFailure &)
			{
			}
		}

		return res;
	}

	void CFES::SecureGenerate(SecureVector<byte> &Output, size_t Offset, size_t Length)
	{
		ACP gen;

		gen.Generate(Output, Offset, Length);
	}
}