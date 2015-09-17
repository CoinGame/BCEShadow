Feature: Shareholders can vote for assets
  Scenario: A shareholder votes for an asset
    Given a network at protocol 3.1 with nodes "Alice" and "Bob"
    And node "Alice" votes for the following assets:
      | BlockchainId | AssetId | Confirmations | M | N |      MaxTrade |
      |            1 |       0 |            60 | 2 | 3 |  300000000000 |
      |            1 |       1 |            60 | 2 | 3 | 6000000000000 |
      |            2 |       0 |             6 | 2 | 3 |   10000000000 |
    And node "Bob" finds 2 blocks

    When node "Alice" finds a block "A"
    Then block "A" on node "Alice" should contain the following asset votes:
      | BlockchainId | AssetId | Confirmations | M | N |      MaxTrade |
      |            1 |       0 |            60 | 2 | 3 |  300000000000 |
      |            1 |       1 |            60 | 2 | 3 | 6000000000000 |
      |            2 |       0 |             6 | 2 | 3 |   10000000000 |
    And the active assets in node "Bob" are empty

    When node "Alice" finds a block "B"
    Then block "B" on node "Alice" should contain the following asset votes:
      | BlockchainId | AssetId | Confirmations | M | N |      MaxTrade |
      |            1 |       0 |            60 | 2 | 3 |  300000000000 |
      |            1 |       1 |            60 | 2 | 3 | 6000000000000 |
      |            2 |       0 |             6 | 2 | 3 |   10000000000 |
    And the active assets in node "Bob" are empty

    When node "Alice" finds a block "C"
    Then block "C" on node "Alice" should contain the following asset votes:
      | BlockchainId | AssetId | Confirmations | M | N |      MaxTrade |
      |            1 |       0 |            60 | 2 | 3 |  300000000000 |
      |            1 |       1 |            60 | 2 | 3 | 6000000000000 |
      |            2 |       0 |             6 | 2 | 3 |   10000000000 |
    And the active assets in node "Bob" are empty

    When node "Alice" finds a block "D" received by all nodes
    #Then block "D" on node "Alice" should contain no asset votes
    And the active assets in node "Bob" are empty

    When node "Alice" finds a block "E" received by all nodes
    #Then block "E" on node "Alice" should contain no asset votes
    And the active assets in node "Bob" are empty

    When node "Alice" finds a block "F" received by all nodes
    Then block "F" on node "Alice" should contain no asset votes
    And the active assets in node "Bob" are empty

    When node "Alice" finds a block "G" received by all nodes
    Then block "G" on node "Alice" should contain no asset votes
    And the active assets in node "Bob" are empty

    When node "Alice" finds a block "H" received by all nodes
    Then block "H" on node "Alice" should contain no asset votes
    And the active assets in node "Bob" are:
      | BlockchainId | AssetId | Confirmations | M | N |      MaxTrade |
      |            1 |       0 |            60 | 2 | 3 |  300000000000 |
      |            1 |       1 |            60 | 2 | 3 | 6000000000000 |
      |            2 |       0 |             6 | 2 | 3 |   10000000000 |