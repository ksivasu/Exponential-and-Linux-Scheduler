class Solution {
    public int[] plusOne(int[] digits) {
        int i=digits.length - 1;
        while (digits[i] == 9)
        {
            digits[i] = 0;
            if(i==0)
            {
                int[] a_new = new int[digits.length + 1];
                    a_new[0] = 1;
                for(i=1; i<digits.length+1; i++)
                {
                    a_new[i]=digits[i-1];
                }
                return a_new;
            }
            i--;
            }
            digits[i]=digits[i]+1;
            return digits;
    }


    }
