Feature:
  The network should handle a fork that happens before a custodian grant and is resolved after it.
  Keeping track of the already elected custodians may make this fail.

  Scenario: The network forks around a custodian grant
    Given a network with nodes "Alice" and "Bob" able to mint
    And a node "Custodian"
    When node "Custodian" generates a NuBit address "cust"
    And node "Bob" ignores all new blocks
    And node "Alice" finds 3 blocks
    And node "Alice" votes an amount of "1,000" for custodian "cust"
    And node "Alice" finds blocks until custodian "cust" is elected
    And node "Bob" stops ignoring new blocks
    And node "Bob" votes an amount of "1,000" for custodian "cust"
    And node "Bob" finds blocks until custodian "cust" is elected
    And node "Bob" finds 5 blocks
    And node "Bob" finds a block "X"
    Then node "Alice" should reach block "X"
