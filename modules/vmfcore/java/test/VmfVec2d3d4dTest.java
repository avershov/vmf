import com.intel.vmf.Vmf;
import com.intel.vmf.vmf_vec2d;
import com.intel.vmf.vmf_vec3d;
import com.intel.vmf.vmf_vec4d;

import static org.junit.Assert.*;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

public class VmfVec2d3d4dTest 
{
    @BeforeClass
    public static void init()
    {
        Vmf.initialize();
    }
    
    @AfterClass
    public static void terminate()
    {
        Vmf.terminate();
    }
    
    protected vmf_vec2d v2d;
    protected vmf_vec3d v3d;
    protected vmf_vec4d v4d;
    
    protected vmf_vec2d v2d1;
    protected vmf_vec3d v3d1;
    protected vmf_vec4d v4d1;
    
	@Test
    public void testVmfVec2d3d4d ()
    {
        v2d = new vmf_vec2d (1, 2);
        v3d = new vmf_vec3d (1, 2, 3);
        v4d = new vmf_vec4d (1, 2, 3, 4);
        
        v2d1 = new vmf_vec2d ();
        v3d1 = new vmf_vec3d ();
        v4d1 = new vmf_vec4d ();
        
        assertEquals(1, v2d.getX(), 0.00001);
        assertEquals(2, v2d.getY(), 0.00001);
        
        assertEquals(1, v3d.getX(), 0.00001);
        assertEquals(2, v3d.getY(), 0.00001);
        assertEquals(3, v3d.getZ(), 0.00001);
        
        assertEquals(1, v4d.getX(), 0.00001);
        assertEquals(2, v4d.getY(), 0.00001);
        assertEquals(3, v4d.getZ(), 0.00001);
        assertEquals(4, v4d.getW(), 0.00001);
        
        assertEquals(0, v2d1.getX(), 0.00001);
        assertEquals(0, v2d1.getY(), 0.00001);
        
        assertEquals(0, v3d1.getX(), 0.00001);
        assertEquals(0, v3d1.getY(), 0.00001);
        assertEquals(0, v3d1.getZ(), 0.00001);
        
        assertEquals(0, v4d1.getX(), 0.00001);
        assertEquals(0, v4d1.getY(), 0.00001);
        assertEquals(0, v4d1.getZ(), 0.00001);
        assertEquals(0, v4d1.getW(), 0.00001);
    }
}
