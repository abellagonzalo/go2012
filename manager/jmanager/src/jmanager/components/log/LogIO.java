package jmanager.components.log;

import java.io.DataInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 *
 * @author caguero
 */
public class LogIO {

    public enum LogDataType {

        LOG_BOOL,
        LOG_BYTE,
        LOG_SHORT,
        LOG_INT,
        LOG_LONG,
        LOG_FLOAT,
        LOG_DOUBLE,
        LOG_STRING
    };

    public LogIO(DataInputStream input) {
        _in = input;       
    }

    public int readInt() throws IOException {
        return intToBigEndian(_in.readInt());
    }

    public int readShort() throws IOException {
        return shortToBigEndian(_in.readShort());
    }

    public final long readLong() throws IOException {
        return longToBigEndian((long) (_in.readInt()) << 32) + (_in.readInt() & 0xFFFFFFFFL);
    }
    
    public final double readDouble() throws IOException {
        ByteBuffer buf = ByteBuffer.allocate(8);
        buf.order(ByteOrder.LITTLE_ENDIAN);  
        buf.putDouble(_in.readDouble());       
        buf.order(ByteOrder.BIG_ENDIAN);  
        return buf.getDouble(0);
    }
    
    public final float readFloat() throws IOException {
        ByteBuffer buf = ByteBuffer.allocate(4);
        buf.order(ByteOrder.LITTLE_ENDIAN);  
        buf.putFloat(_in.readFloat());       
        buf.order(ByteOrder.BIG_ENDIAN);  
        return buf.getFloat(0);
    }

    public LogDataType readDataType() throws IOException {
        int aux = readInt();
        return LogDataType.values()[aux];
    }

    public String readString(int length) throws IOException {
        String str = "";
        for (int i = 0; i < length; i++) {
            byte b = _in.readByte();
            char c = (char) b;
            str += c;
        }
        return str;
    }

    private int intToBigEndian(int v) {
        return ((v & 0xff) << 24) + ((v & 0xff00) << 8) + ((v & 0xff0000) >> 8) + ((v >> 24) & 0xff);
    }

    private int shortToBigEndian(int v) {
        return ((v >> 8) & 0xff) + ((v << 8) & 0xff00);
    }
    
    private long longToBigEndian(long v) {
        ByteBuffer buf = ByteBuffer.allocate(8);
        buf.order(ByteOrder.LITTLE_ENDIAN);  
        buf.putLong(v);       
        buf.order(ByteOrder.BIG_ENDIAN);        
        return buf.getLong(0);
    }
    
    private DataInputStream _in;
}