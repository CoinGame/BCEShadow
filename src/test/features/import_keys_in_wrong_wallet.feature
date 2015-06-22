Feature: When a private key is imported into a wallet, transactions from other units must be ignored
  Scenario:
    Given a network with nodes "Alice"
    And a node "Bob"
    And node "Alice" grants herself "1,000" BlockCredits
    When node "Bob" imports the private key "QhoWjbTAAcyQYQC9JJWXs1K6xs9urFGeWyNhQj1KjVqn8C5HMtZh" into the BKS wallet
    And node "Bob" imports the private key "QhoWjbTAAcyQYQC9JJWXs1K6xs9urFGeWyNhQj1KjVqn8C5HMtZh" into the BKC wallet
    And node "Alice" sends "20" BKS to "tLGNLqFgbLhsph875TJATbERTYytHxB2uh"
    And node "Alice" sends "15" BKC to "c9aaeg4EAcZDVPEwJuBayjZTmid5h6yL7m"
    And node "Alice" finds a block received by all nodes
    Then node "Bob" should have 1 BKS transaction
    And the transaction should be a receive of "20"
    Then node "Bob" should have 1 BKC transaction
    And the transaction should be a receive of "15"

    When some time pass
    And node "Bob" sends "3" BKS to node "Alice"
    And node "Bob" sends "4" BKC to node "Alice"
    And node "Alice" reaches 2 transactions in memory pool
    And node "Alice" finds a block received by all nodes
    Then node "Bob" should have 2 BKS transaction
    And the 2nd transaction should be a send of "3"
    Then node "Bob" should have 2 BKC transaction
    And the 2nd transaction should be a send of "4"

    Given a node "Charles"
    When some time pass
    And node "Charles" imports the private key "QhoWjbTAAcyQYQC9JJWXs1K6xs9urFGeWyNhQj1KjVqn8C5HMtZh" into the BKS wallet
    And all nodes reach the same height
    And node "Charles" sends "6" BKS to node "Alice"
    And node "Alice" reaches 1 transaction in memory pool
    And node "Alice" finds a block received by all nodes
    Then node "Bob" should have 3 BKS transaction
    And the 3nd transaction should be a send of "6"
    Then node "Bob" should have 2 BKC transaction
