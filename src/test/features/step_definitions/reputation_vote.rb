When(/^node "(.*?)" votes for the following signers:$/) do |arg1, table|
  node = @nodes[arg1]

  vote = {
    "reputations" => table.hashes.map do |row|
      {
        address: row["Address"],
        weight: row["Number"].to_i,
      }
    end,
  }

  node.rpc("setvote", vote)
  expect(node.rpc("getvote")["reputation"]).to eq(vote["reputation"])
end

Then(/^block "(.*?)" on node "(.*?)" should contain (\d+) reputed signer votes among these:$/) do |arg1, arg2, arg3, table|
  block = @blocks[arg1]
  node = @nodes[arg2]

  info = node.rpc("getblock", block)
  vote = info["vote"]
  reputation_votes = vote["reputations"]
  expect(reputation_votes.size).to eq(arg3.to_i)

  expected_reputation_votes = table.hashes.map do |row|
    {
      "address" => row["Address"],
      "weight" => row["Number"].to_i,
    }
  end

  reputation_votes.each do |reputation_vote|
    expect(expected_reputation_votes).to include(reputation_vote)
  end
end

Then(/^block "(.*?)" on node "(.*?)" should contain (\d+) reputed signer votes$/) do |arg1, arg2, arg3|
  block = @blocks[arg1]
  node = @nodes[arg2]

  info = node.rpc("getblock", block)
  vote = info["vote"]
  reputation_votes = vote["reputations"]
  begin
    expect(reputation_votes.size).to eq(arg3.to_i)
  rescue Exception
    p reputation_votes
    raise
  end
end
