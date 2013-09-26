
package jmanager.VICODE;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author jesus
 */
public class CppParser {

	private static BuilderGUI builderGUI = BuilderGUI.getInstance();

	public CppParser() {
	}

	public static void parseH(String fileNameHPP) throws IOException {
		File input = new File(fileNameHPP);
		FileInputStream fis = null;

		try {
			fis = new FileInputStream(input);
			BufferedInputStream bis = new BufferedInputStream(fis);

			//TODO: UTILIZAR UN BufferedReader EN LUGAR DE DataInputStream
			//BufferedReader br = new BufferedReader(new InputStreamReader(bis));
			DataInputStream dis = new DataInputStream(bis);
			String line;
			while (dis.available() != 0) {
				// this statement reads the line from the file
				line = dis.readLine();

				if (line.indexOf(BuilderGUI.AUXINCLUDE_BEGIN) != -1) {
					builderGUI.setAuxIHCode(readCodeICH(dis));
				}

				if (line.indexOf(BuilderGUI.AUXCODE_BEGIN) != -1) {
					builderGUI.setAuxHCode(readCodeCH(dis));
				}
			}

			fis.close();
			bis.close();
			dis.close();

		} catch (FileNotFoundException ex) {
			Logger.getLogger(CppParser.class.getName()).log(Level.SEVERE, null, ex);
		}
	}

	public static void parseCPP(String fileNameCPP) throws IOException {
		File input = new File(fileNameCPP);
		FileInputStream fis = null;

		try {
			fis = new FileInputStream(input);
			BufferedInputStream bis = new BufferedInputStream(fis);
			DataInputStream dis = new DataInputStream(bis);
			int index = 0;

			//TODO: UTILIZAR UN BufferedReader EN LUGAR DE DataInputStream
			String line;
			while (dis.available() != 0) {
				//this statement reads the line from the file
				line = dis.readLine();

				//Si encuentra justo esa línea (auxinclude begin)
				if (line.indexOf(BuilderGUI.AUXINCLUDE_BEGIN) != -1) {
					
					//Busca y almacena el texto que haya dentro de esa sección.
					//NOTA: Empieza a buscar a partir de la SIGUIENTE línea a la actual,
					//ya que cada llamada a dis.readline() hace que "dis" avance una línea.
					//Con lo cual la línea donde esté el texto "auxinclude begin" se la salta ENTERA.
					builderGUI.setAuxICCode( readCodeICH( dis ) );
				}

				if (line.indexOf(BuilderGUI.AUXCODE_BEGIN) != -1) {
					builderGUI.setAuxCCode(readCodeCH(dis));
				}

				if ( ((index = line.indexOf(BuilderGUI.BEFORE_STATE_CODE)) != -1)
					&& (line.indexOf("_state_code") != -1) ) {

					//System.out.println(index + BuilderGUI.BEFORE_STATE_CODE.length()); //OK.
					String stateId = line.substring(index + BuilderGUI.BEFORE_STATE_CODE.length(),
						line.indexOf("_state_code"));

					String code = readCode(dis);

					State s = null;

					if (stateId.equals("Initial")) {
						s = builderGUI.getInitialState();
					} else {
						s = builderGUI.getState(stateId);
					}

					//Escribe en la ventana del estado 's' el CÓDIGO que hubiera
					//en el .cpp en esa sección (código de un estado).
					s.getStateFrame().setCode(code);
				}

				if ((line.indexOf(BuilderGUI.BEFORE_TRANSITION_CODE) != -1)
					&& (line.indexOf("_transition_code") != -1)) {

					String transitionId = line.substring(index + BuilderGUI.BEFORE_TRANSITION_CODE.length(),
						line.indexOf("_transition_code"));

					String code = readCode(dis);

					Transition t = builderGUI.getTransition(transitionId);

					t.getStateFrame().setCode(code);
				}
			}

			fis.close();
			bis.close();
			dis.close();

		} catch (FileNotFoundException ex) {
			Logger.getLogger(Builder.class.getName()).log(Level.SEVERE, null, ex);
		}
	}

	private static String readSection(DataInputStream dis, String stringMark) {
		String ret = "";
		String codeBeforeComments ="";

		try {
			String line;
			while (dis.available() != 0) {
				line = dis.readLine();

				//Se detiene cuando encuentra justamente la cadena que haya en 'stringMark'.
				//Si hay código delante de la cadena 'stringMark', lo recoge y lo termina con
				//un salto de línea.
				if (line.indexOf(stringMark) != -1) {
					codeBeforeComments = line.substring(0, line.indexOf(stringMark));
					if (codeBeforeComments.length() > 0) {
						ret = ret + line.substring(0, line.indexOf(stringMark)) + "\n";
					}
					break;
				}
				ret = ret + line + "\n";
			}

		} catch (IOException ex) {
			Logger.getLogger(Builder.class.getName()).log(Level.SEVERE, null, ex);
		}

		return ret;
	}

	private static String readCode(DataInputStream dis) {
		return readSection(dis, BuilderGUI.CODE_END);
	}

	private static String readCodeCH(DataInputStream dis) {
		return readSection(dis, BuilderGUI.AUXCODE_END);
	}

	private static String readCodeICH(DataInputStream dis) {
		return readSection(dis, BuilderGUI.AUXINCLUDE_END);
	}
	
}
