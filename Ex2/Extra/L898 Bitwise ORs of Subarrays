class Solution 
{
    public int subarrayBitwiseORs(int[] arr) 
    {
        Set<Integer> ans = new HashSet<>();
        Set<Integer> prev = new HashSet<>();
        for (int num : arr) 
        {
            Set<Integer> curr = new HashSet<>();
            curr.add(num);
            for (int x : prev)
                curr.add(x | num);
            ans.addAll(curr);
            prev = curr;
        }
        return ans.size();
    }
}
