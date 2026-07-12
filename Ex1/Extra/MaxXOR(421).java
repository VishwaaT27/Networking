class Solution 
{
    static class TrieNode 
    {
        TrieNode[] child = new TrieNode[2];
    }
    public int findMaximumXOR(int[] nums) 
    {
        TrieNode root = new TrieNode();
        for (int num : nums) 
        {
            TrieNode curr = root;
            for (int i = 31; i >= 0; i--) 
            {
                int bit = (num >> i) & 1;
                if (curr.child[bit] == null)
                    curr.child[bit] = new TrieNode();
                curr = curr.child[bit];
            }
        }
        int maxXor = 0;
        for (int num : nums) 
        {
            TrieNode curr = root; int xor = 0;
            for (int i = 31; i >= 0; i--) 
            {
                int bit = (num >> i) & 1;
                int opposite = 1 - bit;
                if (curr.child[opposite] != null) 
                {
                    xor |= (1 << i);
                    curr = curr.child[opposite];
                } 
                else curr = curr.child[bit];
            }
            maxXor = Math.max(maxXor, xor);
        }
        return maxXor;
    }
}
