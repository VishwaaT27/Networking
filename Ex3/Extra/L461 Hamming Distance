class Solution 
{
    public int hammingDistance(int x, int y) 
    {
        int c = 0, z = x ^ y;
        while(z > 0)
        {
            z = z & (z-1);
            c++;
        }    
        return c;
    }
}
