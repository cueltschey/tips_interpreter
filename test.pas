PROGRAM FIB;
{ Find the value of fibbonaci numbers! }
VAR
    UNTIL: INTEGER;
    CUR: INTEGER;
    PREV: INTEGER;
    TEMP: INTEGER;
BEGIN
    CUR := 1;
    PREV := 0;
    WRITE('How many fibbonaci numbers do I calculate?');
    READ(UNTIL);
    WRITE('Starting calculation');
    IF UNTIL < 2 THEN
      WRITE('Calculate more please')
    ELSE
      BEGIN
        WRITE('0');
        WRITE('1');
        WHILE UNTIL > 0
        BEGIN
          UNTIL := UNTIL - 1;
          TEMP := CUR;
          CUR := PREV + CUR;
          PREV := TEMP;
          WRITE(CUR)
        END
      END;
    WRITE('Finished!')
END
