# BPLUSTREE

## FORMAT

### header

| offset | size | meaning                                        |
| ------ | ---- | ---------------------------------------------- |
| 0      | 8    | offset of the root block (0 for none)          |
| 8      | 8    | offset of the first deleted block (0 for none) |

### blocks

#### valid block

| offset | size | meaning                                   |
| ------ | ---- | ----------------------------------------- |
| 0      | 1    | Is_internal                               |
| 1      | 4    | degree                                    |
| 5      | 4    | total number of children                  |
| 9      | 8    | offset of left sibling (if not internal)  |
| 17     | 8    | offset of right sibling (if not internal) |
| 25     |      | a few keys and pointers                   |

If internal, there will be `total` pointers and `total - 1` keys. We asssert that `total <= degree`.

Otherwise, there will be `total` pointers and `total` keys as well. We asssert that `total < degree`.

#### invalid block

| offset | size | meaning                                       |
| ------ | ---- | --------------------------------------------- |
| 0      | 8    | offset of the next deleted block (0 for none) |

