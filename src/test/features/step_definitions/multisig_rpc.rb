When(/^the public key of address "(.*?)" is retreived from node "(.*?)"$/) do |arg1, arg2|
  address_name = arg1
  address = @addresses[address_name]
  node = @nodes[arg2]
  @pubkeys[address] = node.rpc("validateaddress", address)["pubkey"]
end

When(/^node "(.*?)" creates a multisig address "(.*?)" requiring (\d+) keys? from the public keys (.*?)$/) do |arg1, arg2, arg3, arg4|
  node = @nodes[arg1]
  address_name = arg2
  required = arg3.to_i
  addresses = arg4.scan(/"(.*?)"/).map(&:first).map { |name| @addresses[name] }
  pubkeys = addresses.map { |address| @pubkeys[address] }

  result = node.rpc("createmultisig", required, pubkeys)
  @addresses[address_name] = result["address"]
end

When(/^node "(.*?)" adds a multisig address "(.*?)" requiring (\d+) keys? from the public keys (.*?)$/) do |arg1, arg2, arg3, arg4|
  node = @nodes[arg1]
  address_name = arg2
  required = arg3.to_i
  addresses = arg4.scan(/"(.*?)"/).map(&:first).map { |name| @addresses[name] }
  pubkeys = addresses.map { |address| @pubkeys[address] }

  result = node.rpc("addmultisigaddress", required, pubkeys)
  @addresses[address_name] = result
end

When(/^node "(.*?)" sends "(.*?)" to "([^"]*?)" in transaction "(.*?)"$/) do |arg1, arg2, arg3, arg4|
  @tx[arg4] = @nodes[arg1].rpc "sendtoaddress", @addresses[arg3], parse_number(arg2)
end

When(/^node "(.*?)" sends "(.*?)" to "([^"]*?)"$/) do |arg1, arg2, arg3|
  @nodes[arg1].rpc "sendtoaddress", @addresses[arg3], parse_number(arg2)
end

When(/^node "(.*?)" finds a block received by all other nodes$/) do |arg1|
  node = @nodes[arg1]
  block = node.generate_stake
  wait_for do
    main = @nodes.values.map(&:top_hash)
    main.all? { |hash| hash == block }
  end
end

When(/^node "(.*?)" generates a raw transaction "(.*?)" to send the amount sent to address "(.*?)" in transaction "(.*?)" to:$/) do |arg1, arg2, arg3, arg4, table|
  node = @nodes[arg1]
  raw_transaction_name = arg2
  address = @addresses[arg3]
  tx = @tx[arg4]

  outputs = []
  source_tx = node.rpc("getrawtransaction", tx, 1)
  source_tx["vout"].each do |output|
    if output["scriptPubKey"]["addresses"] == [address]
      outputs << {"txid" => tx, "vout" => output["n"]}
    end
  end
  raise "No output found to address #{address} in transaction #{tx}" if outputs.empty?

  recipients = {}
  table.hashes.each do |hash|
    address = @addresses[hash["Address"]]
    value = parse_number(hash["Value"])
    recipients[address] = value
  end

  result = node.rpc("createrawtransaction", outputs, recipients)
  @raw_tx[raw_transaction_name] = result
end

When(/^node "(.*?)" signs the raw transaction "(.*?)"$/) do |arg1, arg2|
  node = @nodes[arg1]
  raw_tx = @raw_tx[arg2]
  result = node.rpc("signrawtransaction", raw_tx)
  signed_raw_tx = result["hex"]
  @raw_tx[arg2] = signed_raw_tx
  @raw_tx_complete[arg2] = result["complete"]
end

Then(/^the raw transaction "(.*?)" should be complete$/) do |arg1|
  expect(@raw_tx_complete[arg1]).to eq(true)
end

When(/^node "(.*?)" sends the raw transaction "(.*?)"$/) do |arg1, arg2|
  node = @nodes[arg1]
  raw_tx = @raw_tx[arg2]
  node.rpc("sendrawtransaction", raw_tx, 1)
end

Then(/^node "(.*?)" should reach a balance of "(.*?)"$/) do |arg1, arg2|
  node = @nodes[arg1]
  amount = parse_number(arg2)
  wait_for do
    expect(node.rpc("getbalance")).to eq(amount)
  end
end

Then(/^node "(.*?)" should reach an unspent amount of "(.*?)" on address "(.*?)"$/) do |arg1, arg2, arg3|
  node = @nodes[arg1]
  expected_amount = parse_number(arg2)
  address = @addresses[arg3]
  wait_for do
    unspent = node.rpc("listunspent", 0, 999999, [address])
    amount = unspent.map { |u| u["amount"] }.inject(0, :+)
    expect(amount).to eq(expected_amount)
  end
end


