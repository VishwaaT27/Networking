class Solution 
{
    public String addBinary(String a, String b)
    {
        int i = a.length() - 1;
        int j = b.length() - 1;
        int c = 0;
        int maxLen = Math.max(a.length(), b.length());
        char[] result = new char[maxLen + 1];
        int p = result.length - 1; 
        while (i >= 0 || j >= 0) 
        {
            int x = (i >= 0) ? a.charAt(i--) - '0' : 0;
            int y = (j >= 0) ? b.charAt(j--) - '0' : 0;
            int z = x + y + c;
            result[p--] = (char) ((z % 2) + '0'); 
            c = z / 2;
        }
        if (c == 1) 
        {
            result[p] = '1';
            return new String(result, p, result.length - p); 
        }
        return new String(result, p + 1, result.length - (p + 1));
    }
}
