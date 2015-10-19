Feature: Shareholders can vote for assets
  Scenario: A shareholder votes for an asset
    Given a network at protocol 3.1 with nodes "Alice" and "Bob" and "Eve"
    And node "Alice" votes for the following assets:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 2 | 3 |  300000000000 |
      |    65537 |            60 | 2 | 3 | 6000000000000 |
    And node "Bob" votes for the following assets:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |           120 | 3 | 5 |  100000000000 |
      |   131072 |             6 | 2 | 3 |   10000000000 |

    When node "Alice" finds a block "A" received by all nodes
    And block "A" on node "Alice" should contain the following asset votes:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 2 | 3 |  300000000000 |
      |    65537 |            60 | 2 | 3 | 6000000000000 |
    Then the active assets in node "Eve" are empty

    When node "Bob" finds a block "B" received by all nodes
    And block "B" on node "Alice" should contain the following asset votes:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |           120 | 3 | 5 |  100000000000 |
      |   131072 |             6 | 2 | 3 |   10000000000 |
    Then the active assets in node "Eve" are empty

    When node "Alice" finds 1 blocks received by all nodes
    And node "Eve" finds 3 blocks received by all nodes
    And the active assets in node "Eve" are:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 2 | 3 |  100000000000 |

    When node "Bob" finds 3 blocks received by all nodes
    And node "Eve" finds 3 blocks received by all nodes
    And the active assets in node "Eve" are:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |           120 | 3 | 5 |  100000000000 |
      |   131072 |             6 | 2 | 3 |   10000000000 |


    When node "Alice" finds 3 blocks received by all nodes
    And node "Eve" finds 3 blocks received by all nodes
    And the active assets in node "Eve" are:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 2 | 3 |  300000000000 |
      |    65537 |            60 | 2 | 3 | 6000000000000 |
      |   131072 |             6 | 2 | 3 |   10000000000 |


  Scenario: Shareholders votes for an asset with different values
    Given a network at protocol 3.1 with nodes "Alice" and "Bob" and "Eve"
    And node "Alice" votes for the following assets:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 2 | 3 |  300000000000 |
    And node "Bob" votes for the following assets:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |           120 | 3 | 5 |  100000000000 |
    And node "Eve" votes for the following assets:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 3 | 5 |  200000000000 |

    When node "Alice" finds 2 blocks received by all nodes
    And node "Bob" finds 2 blocks received by all nodes
    And node "Eve" finds 1 blocks received by all nodes
    And node "Eve" resets her vote
    And node "Eve" finds 3 blocks received by all nodes
    And the active assets in node "Eve" are:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 3 | 5 |  200000000000 |

  Scenario: A shareholder updates an asset
    Given a network at protocol 3.1 with nodes "Alice" and "Bob"
    And node "Alice" votes for the following assets:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 2 | 3 |  300000000000 |
    And node "Alice" finds 6 blocks received by all nodes
    And the active assets in node "Bob" are:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |            60 | 2 | 3 |  300000000000 |

    When node "Alice" votes for the following assets:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |           120 | 3 | 5 |  900000000000 |
    And node "Alice" finds 6 blocks received by all nodes
    And the active assets in node "Bob" are:
      | GlobalId | Confirmations | M | N |      MaxTrade |
      |    65536 |           120 | 3 | 5 |  900000000000 |
