# RecordData

## FORMAT

### header

| offset | size | meaning                                    |
| ------ | ---- | ------------------------------------------ |
| 0      | 8    | offset of first valid block (0 for none)   |
| 8      | 8    | offset of first deleted block (0 for none) |

### valid block

| offset | size | meaning                                 |
| ------ | ---- | --------------------------------------- |
| 0      | 8    | offset of next valid block (0 for none) |
| 8      | 4    | total number of records                 |
| 12     |      | a few records...                        |

### deleted block

| offset | size | meaning                                            |
| ------ | ---- | -------------------------------------------------- |
| 0      | 8    | offset of next deleted block (0 for none)          |
| 8      | 4    | total number of records (should always equal to 0) |

