SELECT SUB_FNAME, SUB_LNAME, CSTL_NAM, REG_NAM, PROF_NAME 
FROM SUBJECT
JOIN PROFESSION ON SUB_PROF = PROF_NUM
JOIN REGION ON SUB_REGION = REG_NUM
JOIN CASTLE ON SUB_CSTL = CSTL_NUM
WHERE SUB_NUM = 1
ORDER BY SUB_NUM;
